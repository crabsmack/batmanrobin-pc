#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_0435C0 = 0x0435C0u;

/* Enemy_GasOrb_SpawnPairJets - spawns a gas orb for the stage 4 tube section whose attacks are the two-jet
 * sweep ($440FC) or the single-jet sweeps ($44146), with the slower puffs
 * (cruise y $A00)
 * In:  x/y (d4/d5) = the spawn position (wave entry)
 * Out: script (a2) = Scr2_0435C0, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_0435C0,a2 / jmp Obj_SpawnEnemy; Scr2_0435C0: speed rand $19/$100, score $1F4, energy
 *           $1C0, coroutine 0 $435DE (one of three hover windows), coroutine 1 $4364E (random pick of the two
 *           vector-1 attacks), drop init $4399A, body $41E4A; wave entries at $41D04/$41D0C (Scr2_0417CC tube
 *           wave, x $168 / -$28); TAS 124188-127823 (15 spawns)
 */
void Enemy_GasOrb_SpawnPairJets(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                                uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_0435C0;
    dk::jump(at_Obj_SpawnEnemy);
}
