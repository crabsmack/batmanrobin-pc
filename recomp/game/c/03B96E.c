#include "game.h"

/* the driver's curtain loop after the native's jsr $7FE.w (VM2_Enter) */
static constexpr uint32_t Scr2_03B978 = 0x03B978u;

/* Boss_CatHead_DriverCurtainY - native fragment of the raster driver's curtain loop: the driver's
 * y is the curtain's height, which Boss_CatHead_CurtainRaster squeezes plane B's curtain into
 * In:  driver = the raster driver object (a6)
 * Out: the arena's curtain_h (g_SceneVars+4); then the script at Scr2_03B978
 * Evidence: move.w $42(a6),(g_SceneVars+4).w / jsr ($7FE).w; after Op2_StepVelocity/ApplyMove at
 *           $03B96C in the coroutine started with Boss_CatHead_StartCurtainRaster; TAS
 *           f87285-f90022 (908 frames: the curtain rising, closing and reopening after the defeat)
 */
void Boss_CatHead_DriverCurtainY(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                                 uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5,
                                 const Vm2Obj *driver)
{
    cat_head_arena()->curtain_h = driver->y;
    dk::jump_script(at_VM2_Enter, Scr2_03B978);
}
