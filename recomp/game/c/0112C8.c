#include "game.h"

static constexpr uint32_t Tbl_011974 = 0x011974u;   /* player 1's panel sprite pieces */
static constexpr uint32_t Tbl_0119AC = 0x0119ACu;   /* player 2's */

/* the registers as the 68000 has them through HUD_Update (the panel script's ops run from a table,
 * so its measured contract can't name what they change: every register goes through that call) */
struct Regs {
    uint32_t d[8];
    uint32_t a[7];
};

/* one player's panel: its script, its slide, its sprites */
static void run_panel(Regs &r, PlayerRec *rec, uint32_t pieces)
{
    r.a[6] = dk::addr(rec);
    HUD_RunPanelScript(&r.d[0], &r.d[1], &r.d[2], &r.d[3], &r.d[4], &r.d[5], &r.d[6], &r.d[7], &r.a[0], &r.a[1],
                       &r.a[2], &r.a[3], &r.a[4], &r.a[5], &r.a[6]);
    HUD_StepPanelSlide(rec);
    r.a[0] = pieces;
    HUD_DrawPanelSprites(&r.d[0], &r.d[1], &r.d[2], &r.d[3], &r.d[4], &r.d[5], &r.d[6], &r.d[7], &r.a[0], &r.a[1],
                         &r.a[2], r.a[6]);
}

/* HUD_Update - per-frame HUD work: the player aggregates, the GO! sign, each panel's script,
 * slide and sprites, then the one DMA step g_HUDStep selects (cleared before it runs; a step
 * that wants a successor sets it again).
 * In:  none
 * Out: d0 = the step taken (its jump-table offset); the rest as the step leaves them
 * Evidence: bsr Player_GatherStats / bsr HUD_UpdateGoSign / per record lea g_PlayerN,a6 / bsr
 *           HUD_RunPanelScript / bsr HUD_StepPanelSlide / lea Tbl_011974 or Tbl_0119AC,a0 / bsr
 *           HUD_DrawPanelSprites / move.w (g_HUDStep).l,d0 / clr.w (g_HUDStep).l / jmp
 *           HUD_StepIdle(pc,d0.w); called by the frame start $9B86 every frame
 */
void HUD_Update(uint32_t *d0, uint32_t d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a5)
{
    Regs r = { { *d0, d1, *d2, *d3, *d4, *d5, *d6, *d7 }, { *a0, *a1, *a2, *a3, *a4, a5, 0 } };
    r.d[0] = Player_GatherStats(&r.a[0]);
    HUD_UpdateGoSign(&r.d[0], &r.d[1], &r.d[2], &r.d[3], &r.d[4], &r.d[5], &r.d[6], &r.d[7], &r.a[0], &r.a[1],
                     &r.a[2]);
    run_panel(r, (PlayerRec *)g_Player1, Tbl_011974);
    run_panel(r, (PlayerRec *)g_Player2, Tbl_0119AC);
    uint16_t step = g_HUDStep;
    r.d[0] = (r.d[0] & 0xFFFF0000u) | step;
    g_HUDStep = 0;
    *d0 = r.d[0], *d2 = r.d[2], *d3 = r.d[3], *d4 = r.d[4], *d5 = r.d[5], *d6 = r.d[6], *d7 = r.d[7];
    *a0 = r.a[0], *a1 = r.a[1], *a2 = r.a[2], *a3 = r.a[3], *a4 = r.a[4];
    dk::jump(at_HUD_StepIdle + (uint32_t)(int32_t)(int16_t)step);
}
