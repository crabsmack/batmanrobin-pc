#include "game.h"

/* the shrink loop's script after this native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_029640 = 0x029640u;

/* Boss_BeamShip_DeathRadiusFromX - native in the death disc's shrink loop: the disc's radius = the
 * radius object's x
 * In:  radius = the radius object (a6); the rest its registers
 * Out: g_BeamRadius = its x; the script continues at Scr2_029640 (jsr $7FE.w)
 * Evidence: move.w $3E(a6),g_BeamRadius / jsr $7FE.w; Op2_Native at $029634 after
 *           Op2_SeekScreenX4/StepVelocity/ApplyMove in Scr2_0295D4; TAS f79014-f79108
 */
void Boss_BeamShip_DeathRadiusFromX(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                                    uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5,
                                    const Vm2Obj *radius)
{
    g_BeamRadius = (u16)radius->x;
    dk::jump_script(at_VM2_Enter, Scr2_029640);
}
