#include "game.h"

/* its a4 script (ROM data after the code: nine $5606 forks of the $13252 pickup) */
static constexpr uint32_t Scr4_0130B0 = 0x0130B0u;

/* Drop_HealthA9 - spawn action 40: drops nine A-style health pickups.
 * In:  x/y = the position (d4/d5)
 * Out: script = Scr4_0130B0 (a4), then Drop_RunFxScript
 * Evidence: lea Scr4_0130B0,a4 / bra.w Drop_RunFxScript, Drop_HealthA5's shape; Tbl_012EF0 entry
 *           40; never run in the corpus (a playtest stopped here, "no C for code address $013060",
 *           after the crane boss, 2026-11-17)
 */
void Drop_HealthA9(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *script,
                   uint32_t *a6)
{
    *script = Scr4_0130B0;
    dk::jump(at_Drop_RunFxScript);
}
