#include "game.h"

/* the disc's last a2 script (the bytes after this native's jsr $7FE.w): Section_StartExit, Op2_Kill */
static constexpr uint32_t Scr2_02964C = 0x02964Cu;

/* Boss_BeamShip_ClearRadius - native at the end of the death disc: radius 0 (nothing drawn); the
 * script then ends the flight section and kills the object
 * In:  the radius object's registers
 * Out: g_BeamRadius = 0; the script continues at Scr2_02964C (jsr $7FE.w)
 * Evidence: clr.w g_BeamRadius / jsr $7FE.w; Op2_Native at $029642, followed by `2CBC
 *           Section_StartExit / Op2_Kill`; TAS f79108 (the flight section ends)
 */
void Boss_BeamShip_ClearRadius(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                               uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    g_BeamRadius = 0;
    dk::jump_script(at_VM2_Enter, Scr2_02964C);
}
