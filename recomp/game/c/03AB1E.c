#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_03AB2A = 0x03AB2Au;

/* Enemy_PropToy_SpawnChain4Right - spawner of a propeller toy entering from the right (H-flipped) with
 * the 4-link chain ToyChain_MakeHanging4, course coroutine $03AB42 at cruise $155, body $037E26
 * In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
 * Out: a2 (a2) = Scr2_03AB2A, then Obj_SpawnEnemy's results
 * Evidence: lea Scr2_03AB2A,a2 / jmp Obj_SpawnEnemy; Scr2_03AB2A: Enemy_PropToy_SpawnChain6Right's
 *           script with Op2_SpawnChild $2A $03AF4A and cruise $155 (coroutine 0 $03AB42); the wave
 *           spawn entry at $034730 (x $17C, y $E01A) names it; no corpus run reaches it (raw-stubs
 *           2026-10-30); the contract is Enemy_PropToy_SpawnChain6Right's (the same kind of call);
 *           03AB1E.syn checks it
 */
void Enemy_PropToy_SpawnChain4Right(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *a2 = Scr2_03AB2A;
    dk::jump(at_Obj_SpawnEnemy);
}
