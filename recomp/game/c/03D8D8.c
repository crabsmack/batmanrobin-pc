#include "game.h"

/* the little hat's a4 spawn script (forks Scr4_03D908 20 px up; ROM data after the natives) */
static constexpr uint32_t Scr4_03D8EC = 0x03D8ECu;

/* Boss_Hatter_PopLittleHat - makes a little top hat pop up out of the big hat and fall back in:
 * runs Scr4_03D8EC at the boss's y, passing the boss through g_ArcTargetX for
 * Boss_Hatter_LittleHatSetOwner
 * In:  boss = the Hatter (a6); the rest pass through to Fx_RunScript
 * Out: g_ArcTargetX = the boss's handle (a6's low word), a0 = Scr4_03D8EC, d5 = the boss's y
 *      (16.16); then Fx_RunScript's
 * Evidence: move.w a6,(g_ArcTargetX).w / lea Scr4_03D8EC,a0 / move.l $42(a6),d5 / jmp Fx_RunScript;
 *           dc.l $0003D8D8 at $03D6F4 in Scr2_03D508; TAS f113672. a0 an output (measured as a
 *           clobber): Fx_RunScript, jumped to, takes the script in it
 */
uint32_t Boss_Hatter_PopLittleHat(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d6,
                                  uint32_t d7, uint32_t *a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4,
                                  uint32_t a5, const Vm2Obj *boss)
{
    g_ArcTargetX = (uint16_t)dk::handle_of(boss);
    *a0 = Scr4_03D8EC;
    uint32_t y = ((uint32_t)(uint16_t)boss->y << 16) | boss->y_frac;
    dk::jump(at_Fx_RunScript);
    return y;
}
