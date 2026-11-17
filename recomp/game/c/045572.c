#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_04557E = 0x04557Eu;

/* Enemy_JetTrooper_SpawnSideToSide - spawns a tube-section jetpack trooper at a random speed that alternates
 * between windows right ($50..$A0) and left (-$50..-$A0) of the targeted
 * player
 * In:  x/y (d4/d5) = the spawn position (wave entry, x $168 or -$28)
 * Out: script (a2) = Scr2_04557E, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_04557E,a2 / jmp Obj_SpawnEnemy; Scr2_04557E: Op2_SetSpeedRand $33/$E6, coroutine 0
 *           $455A6: Op2_GosubArgs ($50, -$50, $A0, $50) wait $960, ..., ($50, -$50, -$50, -$A0); wave entries
 *           $41CC4/$41CCC; RAM f122940/f123840 slot 0 $455BE; TAS 122888-127550 (11 spawns)
 */
void Enemy_JetTrooper_SpawnSideToSide(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                                      uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_04557E;
    dk::jump(at_Obj_SpawnEnemy);
}
