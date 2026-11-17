#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_045A0E = 0x045A0Eu;

/* Enemy_VentTank_SpawnFast - spawns a vent tank for the lab's last arena (floor height $9E) that cruises at
 * $D5 and alternates four vent patterns ($45B8C-$45BE0) with $5A0 / $F0 tick waits
 * In:  x/y (d4/d5) = the spawn position (wave entry: x $15E or -$1E)
 * Out: script (a2) = Scr2_045A0E, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_045A0E,a2 / jmp Obj_SpawnEnemy; Scr2_045A0E: score $C8, energy $300, coroutine 0 $45A26
 *           (x accel/cruise $D5, chase gosub, vent gosubs +$100/+$148/+$126/+$134/+$FA/+$120), body $42D74;
 *           wave entries $41AE8/$41AF0 in the arena wave $41A62 (Op5_LockCameraHere); TAS 121763-122237 (3
 *           spawns)
 */
void Enemy_VentTank_SpawnFast(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                              uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_045A0E;
    dk::jump(at_Obj_SpawnEnemy);
}
