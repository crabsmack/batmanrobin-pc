#include "game.h"
#include "platform.h"

/* The per-line V-scroll raster: a RAM H-int handler (g_HIntCode, copied from the ROM template
 * Tbl_HIntHandlerSrc and patched in place: its move's source word g_HIntOperand, its bcs
 * displacement g_HIntBcsDisp) writes one word of a line table to VSRAM on every line. The handler
 * itself stays generated code (ruling 6); these set it up. */

static constexpr uint32_t Tbl_HIntHandlerSrc = 0x00B20Cu;   /* the handler template (data) */
static constexpr int HINT_COPY_WORDS = 0x26;                /* moveq #$25,d7 / dbra: $4C bytes */
static constexpr uint16_t HINT_TBL_BACK = 0x9040;           /* the two line tables, $FF9040 and */
static constexpr uint16_t HINT_TBL_FRONT = 0x9240;          /* $FF9240 (their low words) */
static constexpr uint8_t HINT_BCS_STEP = 2;                 /* bcs back to the high-byte step */

/* HInt_InstallVScrollB - installs the per-line V-scroll raster for plane B: the H-int handler will
 * write one word per line to VSRAM 2.
 * In:  none
 * Out: g_HIntVSRAMCmd = $40020010; then as HInt_Install (end/src/dst = d7/a0/a1)
 * Evidence: move.l #$40020010,(g_HIntVSRAMCmd).w / bra.s HInt_Install; callers $33274, $33322
 *           (stage 2 section code) and $3BB88; 4 corpus runs
 */
void HInt_InstallVScrollB(uint32_t *end, uint32_t *src, uint32_t *dst)
{
    g_HIntVSRAMCmd = 0x40020010u;   /* VSRAM write, address 2 */
    HInt_Install(end, src, dst);
}

/* HInt_InstallVScrollA - installs the per-line V-scroll raster for plane A (VSRAM 0).
 * In:  none
 * Out: g_HIntVSRAMCmd = $40000010; then as HInt_Install (end/src/dst = d7/a0/a1)
 * Evidence: move.l #$40000010,(g_HIntVSRAMCmd).w, falling into HInt_Install; callers $334D2 and
 *           $471C0 (span effects), $405F2, $3BB88
 */
void HInt_InstallVScrollA(uint32_t *end, uint32_t *src, uint32_t *dst)
{
    g_HIntVSRAMCmd = 0x40000010u;   /* VSRAM write, address 0 */
    HInt_Install(end, src, dst);
}

/* HInt_Install - copies the H-int handler template to RAM (g_HIntCode, the IRQ4 vector's target)
 * and sets the two line tables: g_HIntTblBack = $9040 (the one the game fills), g_HIntTblFront =
 * $9240 (the one shown).
 * In:  g_HIntVSRAMCmd set by the caller
 * Out: $26 words at g_HIntCode; end = $FFFF (the dbra's end, d7's low word; its high word 0 from
 *      the moveq); src/dst = past the copy (a0/a1)
 * Evidence: lea Tbl_HIntHandlerSrc,a0 / lea g_HIntCode,a1 / moveq #$25,d7 / move.w (a0)+,(a1)+ /
 *           dbra; move.w #$9040 / #$9240; the tail of HInt_InstallVScrollA/B
 */
void HInt_Install(uint32_t *end, uint32_t *src, uint32_t *dst)
{
    PTR(const u16) template_words;
    template_words.v = Tbl_HIntHandlerSrc;
    const u16 *from = template_words;
    u16 *to = (u16 *)g_HIntCode;
    for (int i = 0; i < HINT_COPY_WORDS; i++)
        *to++ = *from++;
    *end = 0x0000FFFFu;
    *src = Tbl_HIntHandlerSrc + 2 * HINT_COPY_WORDS;
    *dst = ADDR68(to);
    g_HIntTblBack = HINT_TBL_BACK;
    g_HIntTblFront = HINT_TBL_FRONT;
}

/* HInt_Rearm - re-arms the H-int raster on the table already shown: a frame that wasn't rebuilt
 * replays the previous lines.
 * In:  ctrl = the VDP control port (a0)
 * Out: g_HIntOperand = g_HIntTblFront; then as HInt_Arm
 * Evidence: move.w (g_HIntTblFront).w,(g_HIntOperand).w / bra.s HInt_Arm; from the V-int's busy
 *           path (VInt_BusyRearmRaster)
 */
void HInt_Rearm(uint32_t ctrl)
{
    g_HIntOperand = (uint16_t)g_HIntTblFront;
    HInt_Arm(ctrl);
}

/* HInt_SwapRearm - swaps the double-buffered line tables (the one the game just filled becomes
 * g_HIntTblFront) and arms the H-int on it.
 * In:  ctrl = the VDP control port (a0)
 * Out: g_HIntTblFront / g_HIntTblBack exchanged, g_HIntOperand = the new front; then as HInt_Arm
 * Evidence: the exchange of $E552/$E554 through d0/d1 and move.w d0,(g_HIntOperand).w, falling
 *           into HInt_Arm; the span renderers fill the back table; 35 corpus runs
 */
void HInt_SwapRearm(uint32_t ctrl)
{
    uint16_t filled = g_HIntTblBack;
    uint16_t shown = g_HIntTblFront;
    g_HIntTblFront = filled;
    g_HIntTblBack = shown;
    g_HIntOperand = filled;
    HInt_Arm(ctrl);
}

/* HInt_Arm - enables the per-line H-int: restores the handler's continue branch, sets an H-int on
 * every line, auto-increment 0 (each handler write lands on the same VSRAM word: one V-scroll a
 * line) and the display on, and points the VDP at the handler's VSRAM target.
 * In:  ctrl = the VDP control port (a0); g_HIntOperand = the table
 * Out: VDP reg 0 = $14, reg 10 = 0, reg 15 = 0, reg 1 = $64, the VDP address = g_HIntVSRAMCmd
 * Evidence: move.b #2,(g_HIntBcsDisp).w / move.l #$80148A00,(a0) / move.l #$8F008164,(a0) /
 *           move.l (g_HIntVSRAMCmd).w,(a0); 35 corpus runs
 */
void HInt_Arm(uint32_t ctrl)
{
    g_HIntBcsDisp = HINT_BCS_STEP;
    hw::port_write32(ctrl, 0x80148A00u);   /* H-int on; H-int counter 0: every line */
    hw::port_write32(ctrl, 0x8F008164u);   /* auto-increment 0; display on, V-int on */
    hw::port_write32(ctrl, g_HIntVSRAMCmd);
}
