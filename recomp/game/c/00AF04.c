#include "game.h"

/* The V-int scroll modes 0-$10 ($00AF04-$00AF55): what each slot of VInt_ScrollModeTbl runs.
 * VInt_Handler turned the display off for the V-int (reg 1 = $34); each mode turns it back on.
 * V-int code: D2 checks it. */

enum {
    VINT_DISPLAY_ON = 0x8164,          /* reg 1: display on, V-int on, DMA off */
};
static constexpr uint32_t VINT_HSCROLL_WRITE = 0x7C000002u;   /* VRAM write $BC00 (H-scroll table) */
static constexpr uint32_t VINT_VSRAM_WRITE = 0x40000010u;     /* VSRAM write 0 */

/* both planes' scroll words as the one long the asm moves (plane A high, plane B low) */
static uint32_t scroll_pair(const u16 &a, const u16 &b)
{
    return (uint32_t)(uint16_t)a << 16 | (uint16_t)b;
}

/* VInt_ScrollDisplayOn - scroll mode 0, and the tail of every mode: the display back on.
 * In:  a0 = VDP control port
 * Out: reg 1 = $64
 * Evidence: move.w #$8164,(a0) / rts; mode 0 in both tables and the busy slots; 124 corpus runs
 */
void VInt_ScrollDisplayOn(uint32_t ctrl)
{
    hw::port_write16(ctrl, VINT_DISPLAY_ON);
}

/* VInt_ScrollCustom - scroll mode 4: the scene's own V-int scroll routine.
 * In:  a0/a1 = VDP ports (handed on); g_ScrollVIntFn = the routine
 * Out: whatever the routine does (it turns the display back on itself)
 * Evidence: movea.l g_ScrollVIntFn,a4 / jmp (a4); $3BB88 sets mode 4 with $E55C = $3BBAC /
 *           $3BC56 / $3BCEC; 4 corpus runs
 */
void VInt_ScrollCustom(void)
{
    dk::jump(dk::code((uint32_t)g_ScrollVIntFn));
}

/* VInt_ScrollPlanes - scroll mode 8: whole-plane scroll, the H-scroll long to VRAM $BC00 and the
 * V-scroll long to VSRAM 0.
 * In:  a0/a1 = VDP control/data ports; g_HScrollA/B, g_VScrollA/B
 * Out: reg 1 = $64
 * Evidence: move.l #$7C000002,$C00004 / move.l g_HScrollA,(a1) / move.l #$40000010,(a0) / move.l
 *           g_VScrollA,(a1) / move.w #$8164,(a0); 21 corpus runs
 */
void VInt_ScrollPlanes(uint32_t ctrl, uint32_t data)
{
    hw::port_write32(hw::VDP_CTRL, VINT_HSCROLL_WRITE);   /* the absolute port, not a0 */
    hw::port_write32(data, scroll_pair(g_HScrollA, g_HScrollB));
    hw::port_write32(ctrl, VINT_VSRAM_WRITE);
    hw::port_write32(data, scroll_pair(g_VScrollA, g_VScrollB));
    hw::port_write16(ctrl, VINT_DISPLAY_ON);
}

/* VInt_ScrollLines - scroll mode $C (the most used): the V-scroll long, then both planes'
 * per-line H-scroll by DMA.
 * In:  a0/a1 = VDP ports, a2 = the DMA command scratch
 * Out: reg 1 = $64
 * Evidence: move.l #$40000010,(a0) / move.l g_VScrollA,(a1) / bsr Scroll_DMALines / move.w
 *           #$8164,(a0); 92 corpus runs
 */
void VInt_ScrollLines(uint32_t ctrl, uint32_t data, uint32_t scratch)
{
    hw::port_write32(ctrl, VINT_VSRAM_WRITE);
    hw::port_write32(data, scroll_pair(g_VScrollA, g_VScrollB));
    Scroll_DMALines(ctrl, scratch);
    hw::port_write16(ctrl, VINT_DISPLAY_ON);
}

/* VInt_ScrollLinePairs - scroll mode $10: the V-scroll long, then one DMA of the interleaved
 * (A, B) line H-scroll pairs.
 * In:  a0/a1 = VDP ports, a2 = the DMA command scratch
 * Out: reg 1 = $64
 * Evidence: move.l #$40000010,(a0) / move.l g_VScrollA,(a1) / bsr Scroll_DMALinePairs / move.w
 *           #$8164,(a0); 10 corpus runs (the stage 4 end)
 */
void VInt_ScrollLinePairs(uint32_t ctrl, uint32_t data, uint32_t scratch)
{
    hw::port_write32(ctrl, VINT_VSRAM_WRITE);
    hw::port_write32(data, scroll_pair(g_VScrollA, g_VScrollB));
    Scroll_DMALinePairs(ctrl, scratch);
    hw::port_write16(ctrl, VINT_DISPLAY_ON);
}
