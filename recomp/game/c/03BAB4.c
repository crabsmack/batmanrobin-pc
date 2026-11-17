#include "game.h"

/* one coordinate (16.16) moved toward 0 by step, clamped at 0 (the asm's bpl/add/sub/moveq #0;
   the sign tests are the N flag of the wrapped 32-bit result) */
static int32_t settle_toward_zero(int32_t v, int32_t step)
{
    if (v >= 0) {
        v = (int32_t)((uint32_t)v - (uint32_t)step);
        return v >= 0 ? v : 0;
    }
    v = (int32_t)((uint32_t)v + (uint32_t)step);
    return v < 0 ? v : 0;
}

/* Boss_CatHead_DriverSettle - moves the raster driver's x and y (16.16) toward 0 by its motion
 * block's accelerations, so the face's sway and bulge die away after the defeat
 * In:  motion = the driver's motion block (a3: accel_x, accel_y); driver = the driver (a6)
 * Out: driver x/y (16.16) moved; d4/d5 = the new x/y; d0 = 0 settled (both 0), 1 not yet
 *      (Op2_CallNativeRel ignores it)
 * Evidence: move.l $20(a3),d0 / move.l $3E(a6),d4 / bpl / add.l or sub.l / moveq #0,d4, the same
 *           for $28(a3)/$42(a6) into d5, or.l d5,d0 / moveq #1,d0; Op2_CallNativeRel at $03BAAE
 *           after Op2_SetAccelForDist $D5/$30; TAS f89343-f89520 (178 frames)
 */
void Boss_CatHead_DriverSettle(uint32_t *d0, uint32_t *d4, uint32_t *d5, const Vm2Motion *motion, Vm2Obj *driver)
{
    int32_t x = (int32_t)((uint32_t)(uint16_t)driver->x << 16 | driver->x_frac);
    x = settle_toward_zero(x, motion->accel_x);
    driver->x = (int16_t)((uint32_t)x >> 16);
    driver->x_frac = (uint16_t)x;

    int32_t y = (int32_t)((uint32_t)(uint16_t)driver->y << 16 | driver->y_frac);
    y = settle_toward_zero(y, motion->accel_y);
    driver->y = (int16_t)((uint32_t)y >> 16);
    driver->y_frac = (uint16_t)y;

    *d4 = (uint32_t)x;
    *d5 = (uint32_t)y;
    *d0 = ((uint32_t)x | (uint32_t)y) != 0 ? 1u : 0u;
}
