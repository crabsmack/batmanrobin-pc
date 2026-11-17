#include "game.h"

/* Frame_Begin - starts a non-gameplay frame: marks it busy, resets the DMA queue and the sprite
 * list and adds the scene's fixed sprites; no HUD (Frame_BeginStage's first three calls).
 * In:  none
 * Out: g_FrameBusy set; d0/d1/d7/a1 as the calls hand them back
 * Evidence: st.b g_FrameBusy / jsr DMA_ResetQueue / Spr_ResetList / Spr_MaskLeftColumn / rts;
 *           caller $47C6A (the intro scene loop: Frame_Begin, Obj_RunList, Camera_UpdateView,
 *           Frame_End)
 */
void Frame_Begin(uint32_t *d0, uint32_t *d1, uint32_t *d7, uint32_t *a1)
{
    uint32_t a0 = 0;
    g_FrameBusy = 0xFF;
    DMA_ResetQueue();
    Spr_ResetList(&a0, a1);
    Spr_MaskLeftColumn(d0, d1, d7, a0);
}
