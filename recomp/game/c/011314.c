#include "game.h"

/* The HUD's per-frame DMA steps ($011314-$0114D1): the jump table's slots 4-$14 (one bra each)
 * and the steps themselves. HUD_Update clears g_HUDStep and jumps to HUD_StepIdle + the old
 * value; each step queues one DMA and names its successor: 4 (the art, once) -> 8 -> $C -> $10 ->
 * $14 -> 8 ... */

/* the panel tables (ROM) */
static constexpr uint32_t Tbl_0586F2 = 0x0586F2u;   /* font glyph -> tile index (ASCII - $20) */
static constexpr uint32_t Art_058AA2 = 0x058AA2u;   /* the HUD's tiles */
static constexpr uint32_t Tbl_051792 = 0x051792u;   /* player 1's charge bar tile-index map */
static constexpr uint32_t Tbl_052512 = 0x052512u;   /* player 1's charge bar tiles */
static constexpr uint32_t Tbl_054812 = 0x054812u;   /* player 2's charge bar tile-index map */
static constexpr uint32_t Tbl_055592 = 0x055592u;   /* player 2's charge bar tiles */

/* the step numbers (offsets into the jump table at HUD_StepIdle) */
enum { HUD_STEP_P1_RING = 8, HUD_STEP_P1_PANEL = 0xC, HUD_STEP_P2_RING = 0x10, HUD_STEP_P2_PANEL = 0x14 };

/* where the score row's padding starts: player 1's after its digits, player 2's (right-aligned) at
 * the buffer's start */
enum { HUD_SCORE_LEFT = 0, HUD_SCORE_RIGHT = 0x60 };

/* the registers as the 68000 has them through a step (the slots pass every one) */
struct Regs {
    uint32_t d[8];
    uint32_t a[7];
};

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* reserves a DMA queue entry (DMA_Reserve: d0 = 1 slot, d1 = its cost) and writes it: the length
 * registers $93/$94, the source $95/$96/$97, the VDP command; false when the queue is full */
static bool queue_dma(Regs &r, uint16_t cost, uint32_t length_regs, uint32_t source_regs, uint16_t source_hi,
                      uint32_t command)
{
    r.d[0] = 1;
    r.d[1] = cost;
    DMA_Reserve(&r.d[0], (uint16_t)r.d[1], &r.a[0]);
    if (r.d[0] != 0)   /* bne / bmi: d0 = -1, nothing reserved */
        return false;
    PTR(u16) q;
    q.v = r.a[0];
    *q++ = (uint16_t)(length_regs >> 16);
    *q++ = (uint16_t)length_regs;
    *q++ = (uint16_t)(source_regs >> 16);
    *q++ = (uint16_t)source_regs;
    *q++ = source_hi;
    *q++ = (uint16_t)(command >> 16);
    *q++ = (uint16_t)command;
    r.a[0] = q.raw();
    g_DMAQueuePtr = (uint16_t)r.a[0];
    return true;
}

/* HUD_StepLoadArt - HUD step 4: queues the HUD's fixed art (Art_058452, $150 words) to VRAM $0280,
 * then step 8.
 * In:  none
 * Out: d0 = 0 / -1 (DMA_Reserve), d1 = $15, a0 past the entry; g_HUDStep = 8 once queued; a full
 *      queue leaves it 0 (HUD_Update cleared it): the cycle stops on HUD_StepIdle (the original's)
 * Evidence: moveq #1,d0 / moveq #$15,d1 / jsr DMA_Reserve / bne / $93509401 / $952996C2 / $9702 /
 *           $42800080 / move.w a0,(g_DMAQueuePtr).w / move.w #8,(g_HUDStep).w
 */
static void step_load_art(Regs &r)
{
    if (!queue_dma(r, 0x15, 0x93509401u, 0x952996C2u, 0x9702u, 0x42800080u))
        return;
    g_HUDStep = HUD_STEP_P1_RING;
}

/* HUD_StepP1Ring / HUD_StepP2Ring - HUD steps 8 / $10: unless the panel is in text mode, draws the
 * player's life ring (HUD_DrawLifeRing) and queues its 9 tiles from g_HUDTileBuf ($90 words) to
 * VRAM $0520 / $0640; then the panel step.
 * In:  the record (g_Player1 / g_Player2)
 * Out: a6 = the record; as HUD_DrawLifeRing and DMA_Reserve leave the rest; g_HUDStep = next
 * Evidence: lea g_Player1,a6 / btst #7,$25(a6) / bne / bsr HUD_DrawLifeRing / moveq #1,d0 / moveq
 *           #$A,d1 / jsr DMA_Reserve / bmi / $93909400 / $951296F5 / $977F / $45200080 (P2
 *           $46400080) / move.w #$C (P2 #$14),(g_HUDStep).w
 */
static void step_ring(Regs &r, u8 *record, uint32_t command, uint16_t next)
{
    r.a[6] = dk::addr(record);
    PlayerRec *rec = (PlayerRec *)record;
    if (!(rec->flags & PLAYER_PANEL_TEXT)) {
        HUD_DrawLifeRing(&r.d[6], &r.d[7], &r.a[0], &r.a[1], &r.a[2], &r.a[3], &r.a[4], r.a[6]);
        queue_dma(r, 0xA, 0x93909400u, 0x951296F5u, 0x977Fu, command);
    }
    g_HUDStep = next;
}

/* HUD_StepP1Panel / HUD_StepP2Panel - HUD steps $C / $14: draws the player's panel into
 * g_HUDTileBuf (a pending text +$1A through HUD_DrawPanelText, then cleared; or the score, its
 * padding and the charge bar) and queues its 18 tiles ($120 words) to VRAM $0760 / $09A0; text mode
 * with nothing pending draws and queues nothing. Then the next ring step.
 * In:  the record (g_Player1 / g_Player2); the score alignment (d4 = 0 / $60) and the bar tables
 * Out: a6 = the record; as the drawing calls and DMA_Reserve leave the rest; g_HUDStep = next
 * Evidence: lea g_Player1,a6 / lea g_HUDTileBuf,a2 / btst #7,$25(a6) / move.l $1A(a6),d0 / beq /
 *           lea Tbl_0586F2,a0 / lea Art_058AA2,a1 / movea.l d0,a3 / bsr HUD_DrawPanelText / clr.l
 *           $1A(a6); moveq #0,d4 (P2 move.w #$60,d4) / bsr HUD_DrawScore / (P2 lea g_HUDTileBuf,a2)
 *           / bsr HUD_ClearTiles / lea Tbl_051792,a0 / lea Tbl_052512,a1 (P2 Tbl_054812 /
 *           Tbl_055592) / bsr HUD_DrawWeaponBar; moveq #1,d0 / moveq #$13,d1 / DMA_Reserve / bmi /
 *           $93209401 / $951296F5 / $977F / $47600080 (P2 $49A00080)
 */
static void step_panel(Regs &r, u8 *record, bool right, uint32_t bar_map, uint32_t bar_tiles, uint32_t command,
                       uint16_t next)
{
    r.a[6] = dk::addr(record);
    PlayerRec *rec = (PlayerRec *)record;
    r.a[2] = dk::addr(g_HUDTileBuf);
    if (rec->flags & PLAYER_PANEL_TEXT) {
        r.d[0] = rec->panel_text;
        if (r.d[0] == 0) {
            g_HUDStep = next;
            return;
        }
        r.a[0] = Tbl_0586F2;
        r.a[1] = Art_058AA2;
        r.a[3] = r.d[0];
        HUD_DrawPanelText(&r.d[6], &r.d[7], r.a[0], r.a[1], &r.a[2], &r.a[3], &r.a[4]);
        rec->panel_text = 0;
    } else {
        if (right)
            set_low_word(&r.d[4], HUD_SCORE_RIGHT);
        else
            r.d[4] = HUD_SCORE_LEFT;   /* moveq */
        HUD_DrawScore(&r.d[2], &r.d[3], r.d[4], &r.d[7], &r.a[2], r.a[6]);
        if (right)
            r.a[2] = dk::addr(g_HUDTileBuf);
        HUD_ClearTiles((uint16_t)r.d[7], r.a[2]);
        r.a[0] = bar_map;
        r.a[1] = bar_tiles;
        HUD_DrawWeaponBar(&r.d[6], &r.d[7], &r.a[0], r.a[1], &r.a[2], &r.a[3], &r.a[4], r.a[6]);
    }
    queue_dma(r, 0x13, 0x93209401u, 0x951296F5u, 0x977Fu, command);
    g_HUDStep = next;
}

/* HUD_StepBranches - the HUD step jump table's slot 4: HUD_StepLoadArt.
 * In:  a0 (kept when the queue is full)
 * Out: d0, a0 as HUD_StepLoadArt (d1 = $15)
 * Evidence: bra.w HUD_StepLoadArt at offset 4 of jmp HUD_StepIdle(pc,d0.w) at $1130C; HUD_Init
 *           sets step 4
 */
void HUD_StepBranches(uint32_t *d0, uint32_t *a0)
{
    Regs r = {};
    r.d[0] = *d0;
    r.a[0] = *a0;
    step_load_art(r);
    *d0 = r.d[0];
    *a0 = r.a[0];
}

static Regs load(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                 uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5,
                 uint32_t *a6)
{
    return Regs{ { *d0, *d1, *d2, *d3, *d4, *d5, *d6, *d7 }, { *a0, *a1, *a2, *a3, *a4, *a5, *a6 } };
}

static void store(const Regs &r, uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                  uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4,
                  uint32_t *a5, uint32_t *a6)
{
    *d0 = r.d[0], *d1 = r.d[1], *d2 = r.d[2], *d3 = r.d[3], *d4 = r.d[4], *d5 = r.d[5], *d6 = r.d[6], *d7 = r.d[7];
    *a0 = r.a[0], *a1 = r.a[1], *a2 = r.a[2], *a3 = r.a[3], *a4 = r.a[4], *a5 = r.a[5], *a6 = r.a[6];
}

/* HUD_StepToP1Ring - the jump table's slot 8: HUD_StepP1Ring (player 1's life ring to VRAM
 * $0520, then step $C).
 * In:  every register (passed through)
 * Out: as HUD_StepP1Ring
 * Evidence: bra.w HUD_StepP1Ring at $11318, offset 8 of jmp HUD_StepIdle(pc,d0.w)
 */
void HUD_StepToP1Ring(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                      uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                      uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    Regs r = load(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
    step_ring(r, g_Player1, 0x45200080u, HUD_STEP_P1_PANEL);
    store(r, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
}

/* HUD_StepToP1Panel - the jump table's slot $C: HUD_StepP1Panel (player 1's panel, score
 * left-aligned, to VRAM $0760, then step $10).
 * In:  every register (passed through)
 * Out: as HUD_StepP1Panel
 * Evidence: bra.w HUD_StepP1Panel at $1131C, offset $C of jmp HUD_StepIdle(pc,d0.w)
 */
void HUD_StepToP1Panel(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                       uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                       uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    Regs r = load(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
    step_panel(r, g_Player1, false, Tbl_051792, Tbl_052512, 0x47600080u, HUD_STEP_P2_RING);
    store(r, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
}

/* HUD_StepToP2Ring - the jump table's slot $10: HUD_StepP2Ring (player 2's life ring to VRAM
 * $0640, then step $14).
 * In:  every register (passed through)
 * Out: as HUD_StepP2Ring
 * Evidence: bra.w HUD_StepP2Ring at $11320, offset $10 of jmp HUD_StepIdle(pc,d0.w)
 */
void HUD_StepToP2Ring(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                      uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                      uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    Regs r = load(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
    step_ring(r, g_Player2, 0x46400080u, HUD_STEP_P2_PANEL);
    store(r, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
}

/* HUD_StepToP2Panel - the jump table's slot $14: HUD_StepP2Panel (player 2's panel, score
 * right-aligned, to VRAM $09A0, then back to step 8).
 * In:  every register (passed through)
 * Out: as HUD_StepP2Panel
 * Evidence: bra.w HUD_StepP2Panel at $11324, offset $14 of jmp HUD_StepIdle(pc,d0.w)
 */
void HUD_StepToP2Panel(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                       uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                       uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    Regs r = load(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
    step_panel(r, g_Player2, true, Tbl_054812, Tbl_055592, 0x49A00080u, HUD_STEP_P1_RING);
    store(r, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
}
