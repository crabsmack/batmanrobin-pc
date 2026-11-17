#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_045702 = 0x045702u;

/* Enemy_JetTrooper_SpawnBelow - spawns a tube-section jetpack trooper (random speed) that alternates between
 * windows below-right and below-left of the targeted player (y words $64/$5A)
 * In:  x/y (d4/d5) = the spawn position (wave entry, x $168 or -$28)
 * Out: script (a2) = Scr2_045702, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_045702,a2 / jmp Obj_SpawnEnemy; Scr2_045702: coroutine 0 $4572A: Op2_GosubArgs ($64,
 *           $5A, $A0, $50) / ($64, $5A, -$50, -$A0); wave entries $41CF4/$41CFC; RAM f125460/f125880/f128100
 *           slot 0 $45742 at y 198-200 (low on the screen); TAS 125392-128368 (4 spawns)
 */
void Enemy_JetTrooper_SpawnBelow(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                                 uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_045702;
    dk::jump(at_Obj_SpawnEnemy);
}
