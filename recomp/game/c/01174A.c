#include "game.h"

/* The HUD panel script's interpreter ($01174A-$011787): the per-frame step and its NEXT. A panel
 * script is a list of op words (offsets into Tbl_011764, a long per op) with their operands; an op
 * either continues (jumps back to HUD_PanelNext) or stores the script pointer and a wait in the
 * player record and returns. */

static constexpr uint32_t Tbl_011764 = 0x011764u;   /* the op table: HudOp_Halt, _Wait, _Goto, ... */

/* HUD_RunPanelScript - steps one player's HUD panel script: counts down the wait +$1E; when it
 * reaches 0, runs ops from +$20 until one waits. A timer already 0 means the script is halted.
 * In:  a6 = the player record; every register passes on to the ops
 * Out: as the ops leave it (+$1E/+$20 by the op that waits)
 * Evidence: tst.w $1E(a6) / beq rts / subq.w #1,$1E(a6) / beq / movea.l $20(a6),a0, falling into
 *           HUD_PanelNext; called by HUD_Update for g_Player1 and g_Player2 every frame
 */
void HUD_RunPanelScript(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                        uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                        uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    PTR(PlayerRec) p;
    p.v = *a6;
    PlayerRec *rec = p;
    if (rec->panel_timer == 0)
        return;
    rec->panel_timer -= 1;
    if (rec->panel_timer != 0)
        return;
    *a0 = rec->panel_script;
    HUD_PanelNext(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
}

/* HUD_PanelNext - the panel script's NEXT: the next op word picks the handler in Tbl_011764.
 * In:  a0 = the script, at an op word; a6 = the player record
 * Out: a0 past the op word, d0 low word = the op (a table offset), a1 = the handler, jumped to
 * Evidence: move.w (a0)+,d0 / movea.l Tbl_011764(pc,d0.w),a1 / jmp (a1); entered by
 *           HUD_RunPanelScript and the continuing ops (Goto, SlideOut, SlideIn, WaitSlide, ShowStats)
 */
void HUD_PanelNext(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                   uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                   uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    (void)d1, (void)d2, (void)d3, (void)d4, (void)d5, (void)d6, (void)d7;
    (void)a2, (void)a3, (void)a4, (void)a5, (void)a6;
    PTR(const u16) script;
    script.v = *a0;
    uint16_t op = *script.get();
    *a0 += 2;
    *d0 = (*d0 & 0xFFFF0000u) | op;
    PTR(const u32) handler;   /* the long at Tbl_011764 + the op (a byte offset) */
    handler.v = Tbl_011764 + (uint32_t)(int32_t)(int16_t)op;
    *a1 = *handler.get();
    dk::jump(dk::code(*a1));
}
