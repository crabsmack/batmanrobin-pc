#include "game.h"

/* the chooser's script after the native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_049B7A = 0x049B7Au;   /* jsr $7FE.w at $049B76: VM2_Enter pops $049B7A */

/* PlayerSelect_ToggleHero - player-select native: switches the chosen hero (Batman <-> Robin)
 * after an A/B/C/Up/Down press, then back into the chooser's script.
 * In:  the chooser's registers (the script runs on with them)
 * Out: g_P1IsRobin inverted (not.b: 0 <-> $FF); the script continues at Scr2_049B7A (its return op)
 * Evidence: not.b g_P1IsRobin / jsr $7FE.w; Op2_Native at $49B70; ran only in scenario-robin
 *           (f536), where player 1 then plays Robin
 */
void PlayerSelect_ToggleHero(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                             uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    g_P1IsRobin = (uint8_t)~g_P1IsRobin;
    dk::jump_script(at_VM2_Enter, Scr2_049B7A);
}
