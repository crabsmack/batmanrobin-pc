#include "game.h"

/* the spawned boss's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_046D32 = 0x046D32u;

/* Boss_Freeze_Spawn - trigger callback of the Freeze fight ($4401): spawns Mr. Freeze (boss class) with
 * Scr2_046D32, which starts his movement/minion coroutine 0 ($046D40), places him at
 * (300, 100) and jumps into his body at $04619C
 * In:  trigger entry (Tbl_041138)
 * Out: script (a2) = Scr2_046D32, then Obj_SpawnBoss's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_046D32,a2 / jmp Obj_SpawnBoss; the only entry of Tbl_041138 (count 1; entry $04113A,
 *           callback long $046D26 at $041144); Scr2_046D32 = StartCoroutine0Rel, SetPos $12C $64, Goto
 *           $04619C (score 10000, energy $3000/$4000, BossMeter_Show); coroutine 0 calls Spawn_AtScreenCentre
 *           with the gas-orb spawners $043ACC/$043C74/$043D48; TAS f128791
 */
void Boss_Freeze_Spawn(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *script, uint32_t *a4,
                       uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_046D32;
    dk::jump(at_Obj_SpawnBoss);
}
