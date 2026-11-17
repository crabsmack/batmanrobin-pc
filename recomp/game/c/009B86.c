#include "game.h"

/* Frame_BeginStage - starts a gameplay frame: marks the frame busy, resets the DMA queue and the
 * sprite list, adds the scene's fixed sprites (Spr_MaskLeftColumn), then the HUD, the shot-art
 * keeper and the stage hook.
 * In:  none (the registers pass through to the calls)
 * Out: g_FrameBusy set; the registers as the calls hand them back
 * Evidence: st.b g_FrameBusy / jsr DMA_ResetQueue / Spr_ResetList / Spr_MaskLeftColumn /
 *           HUD_Update / Weapon_KeepShotArt / Frame_BeginStageHook / rts; the first call of every
 *           section frame loop (16 callers $1FB3A-$47BD0)
 */
void Frame_BeginStage(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                      uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5,
                      uint32_t *a6)
{
    g_FrameBusy = 0xFF;
    DMA_ResetQueue();
    Spr_ResetList(a0, a1);
    Spr_MaskLeftColumn(d0, d1, d7, *a0);
    HUD_Update(d0, *d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, *a5);
    Weapon_KeepShotArt(d0, d1, d2, d3, a0, a1, a5, a6);
    Frame_BeginStageHook();
}
