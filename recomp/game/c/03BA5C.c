#include "game.h"

/* the driver's sway loop after the native's jsr $7FE.w (VM2_Enter) */
static constexpr uint32_t Scr2_03BA74 = 0x03BA74u;

/* Boss_CatHead_DriverSway - native fragment of the raster driver's sway loop: its 16.16 y / 16 is
 * the face's bulge, its 16.16 x / 64 the sway's slope (Boss_CatHead_SwayRaster draws both)
 * In:  driver = the raster driver object (a6; x on an oscillator: Op2_AddOsc before it)
 * Out: the arena's bulge (g_SceneVars+$C, long) and shear (+$10, the low word); then the script
 *      at Scr2_03BA74
 * Evidence: move.l $42(a6),d0 / asr.l #4,d0 / move.l d0,(g_SceneVars+$C).w / move.l $3E(a6),d0 /
 *           asr.l #6,d0 / move.w d0,(g_SceneVars+$10).w / jsr ($7FE).w; TAS f87705-f89520 (1816
 *           frames); RAM f88320: +$C $00000880, +$10 $E178
 */
void Boss_CatHead_DriverSway(uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                             uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, const Vm2Obj *driver)
{
    int32_t y = (int32_t)((uint32_t)(uint16_t)driver->y << 16 | driver->y_frac);
    int32_t x = (int32_t)((uint32_t)(uint16_t)driver->x << 16 | driver->x_frac);
    CatHeadArena *arena = cat_head_arena();
    arena->bulge = y >> 4;              /* asr.l: arithmetic */
    arena->shear = (int16_t)(x >> 6);   /* the low word of the shifted long */
    dk::jump_script(at_VM2_Enter, Scr2_03BA74);
}
