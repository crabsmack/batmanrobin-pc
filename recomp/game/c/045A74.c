#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_045A80 = 0x045A80u;

/* Enemy_VentTank_SpawnMid - spawns a vent tank (floor height $9E) that cruises at $AA, stops after $78 ticks
 * and alternates vent patterns with rests of $5A0 / $F0 ticks
 * In:  x/y (d4/d5) = the spawn position (trigger entry)
 * Out: script (a2) = Scr2_045A80, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_045A80,a2 / jmp Obj_SpawnEnemy; Scr2_045A80: score $C8, energy $300, coroutine 0 $45A98
 *           (x $AA, chase gosub, wait $78, accel 0, vent gosubs), body $42D74; Tbl_041000 trigger entries x
 *           4448/4704/4960 (spawn action $12); RAM f121500 slots $45ABA/$45B52; TAS 121342-121524 (3 spawns)
 */
void Enemy_VentTank_SpawnMid(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *script, uint32_t *a4,
                             uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_045A80;
    dk::jump(at_Obj_SpawnEnemy);
}
