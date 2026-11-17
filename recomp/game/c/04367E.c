#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_04368A = 0x04368Au;

/* Enemy_GasOrb_SpawnTripleJets - spawns a tube-section gas orb whose attacks are the jet sequence (jets on
 * one by one, then random singles: $4425E) or the all-three-jets sweeps
 * ($441CE), slower puffs (cruise y $A00)
 * In:  x/y (d4/d5) = the spawn position (wave entry)
 * Out: script (a2) = Scr2_04368A, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_04368A,a2 / jmp Obj_SpawnEnemy; Scr2_04368A: as Enemy_GasOrb_SpawnPairJets with
 *           coroutine 1 $43718 (attacks $4425E / $441CE); wave entries at $41D14/$41D1C; TAS frame 126197
 */
void Enemy_GasOrb_SpawnTripleJets(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                                  uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_04368A;
    dk::jump(at_Obj_SpawnEnemy);
}
