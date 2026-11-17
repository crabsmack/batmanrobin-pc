#include "game.h"

/* the radius object's loop after this native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_0295C6 = 0x0295C6u;

/* Boss_BeamShip_RadiusFromX - native in the radius object's loop: the beam disc's radius = the
 * object's x (eased toward the ship's wanted radius)
 * In:  radius = the radius object (a6); the rest its registers
 * Out: g_BeamRadius = its x; the script continues at Scr2_0295C6 (jsr $7FE.w)
 * Evidence: move.w $3E(a6),g_BeamRadius / jsr $7FE.w; Op2_Native at $0295BA in Scr2_029598;
 *           $33A88 draws the disc with this radius; TAS f77185-f79013
 */
void Boss_BeamShip_RadiusFromX(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                               uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, const Vm2Obj *radius)
{
    g_BeamRadius = (u16)radius->x;
    dk::jump_script(at_VM2_Enter, Scr2_0295C6);
}
