#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_045520 = 0x045520u;

/* Enemy_JetTrooper_SpawnHoverRight - spawns a tube-section jetpack trooper that holds a window right of the
 * targeted player (x $50..$B4), cruise $1AA, turn rate $3AA
 * In:  x/y (d4/d5) = the spawn position (wave entry, x -$28)
 * Out: script (a2) = Scr2_045520, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_045520,a2 / jmp Obj_SpawnEnemy; Scr2_045520: coroutine 0 $45546 with Op2_GosubArgs ($50,
 *           -$50, $B4, $50); wave entry $41CBC (enters from the left edge); TAS 123702-127822 (2 spawns)
 */
void Enemy_JetTrooper_SpawnHoverRight(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                                      uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_045520;
    dk::jump(at_Obj_SpawnEnemy);
}
