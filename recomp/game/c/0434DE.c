#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_0434EA = 0x0434EAu;

/* Enemy_GasOrb_SpawnSideSweep - spawns a tube-section gas orb (body $041E4A, energy $1C0) placed with a
 * wide jitter ($12C, $C8) whose coroutine 1 ($4357A) has no jets: after $1E0 ticks it sweeps right and
 * left (x cruise $455, y $A00, legs of $780 ticks)
 * In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
 * Out: script (a2) = Scr2_0434EA, then Obj_SpawnEnemy's results
 * Evidence: lea Scr2_0434EA,a2 / jmp Obj_SpawnEnemy; Scr2_0434EA: Enemy_GasOrb_SpawnPairJets's opening
 *           (speed $19-$100, score $1F4, energy $1C0), coroutine 0 $4350A (accel/cruise $200),
 *           coroutine 1 $4357A (Op2_CruiseRight / Op2_CruiseLeft, var 3 bit 0), JitterPos $12C $C8,
 *           Goto $41E4A; the wave spawn entries at $0416AC and $0416E8 (x $A0 / $154, y $E070) name it;
 *           no corpus run reaches it (raw-stubs 2026-10-30); the contract is
 *           Enemy_GasOrb_SpawnPairJets's (the same kind of call); 0434DE.syn checks it
 */
void Enemy_GasOrb_SpawnSideSweep(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_0434EA;
    dk::jump(at_Obj_SpawnEnemy);
}
