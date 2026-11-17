#include "game.h"

/* the driver's fall loop after the native's jsr $7FE.w (VM2_Enter) */
static constexpr uint32_t Scr2_03BB06 = 0x03BB06u;

/* Boss_CatHead_DriverFallY - native fragment of the raster driver's fall loop: -(its y) is how far
 * the face plate still stands, which Boss_CatHead_FallRaster tips over onto the floor
 * In:  driver = the raster driver object (a6; Scr2_03BB06 sets y -$AA, then cruise y $5D5)
 * Out: the arena's fall (g_SceneVars+6) = -y; then the script at Scr2_03BB06
 * Evidence: move.w $42(a6),d0 / neg.w d0 / move.w d0,(g_SceneVars+6).w / jsr ($7FE).w; after
 *           Op2_StepVelocity/ApplyMove at $03BAF6; TAS f90041-f90392 (319 frames; shots
 *           f90150/f90300: the face fallen flat)
 */
void Boss_CatHead_DriverFallY(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                              uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5,
                              const Vm2Obj *driver)
{
    cat_head_arena()->fall = (int16_t)-driver->y;
    dk::jump_script(at_VM2_Enter, Scr2_03BB06);
}
