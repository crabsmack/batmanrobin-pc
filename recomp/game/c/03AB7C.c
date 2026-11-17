#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_03AB88 = 0x03AB88u;

/* Enemy_PropToy_SpawnTwinChain8 - spawner of a propeller toy entering from the right (H-flipped) with
 * two 8-link chains (children $2A and $2C, both ToyChain_MakeHanging8B), course coroutine $03ABAC, body
 * $037E26
 * In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
 * Out: a2 (a2) = Scr2_03AB88, then Obj_SpawnEnemy's results
 * Evidence: lea Scr2_03AB88,a2 / jmp Obj_SpawnEnemy; Scr2_03AB88: SetAnim, Op2_SpawnChild $2A $03AF78,
 *           SetAnim, Op2_SpawnChild $2C $03AF78, Op2_ModSprAttr $F008, coroutine 0 $03ABAC; the wave
 *           spawn entry at $0347B8 (x $168, y $E010) names it; no corpus run reaches it (raw-stubs
 *           2026-10-30); the contract is Enemy_PropToy_SpawnChain6Right's (the same kind of call);
 *           03AB7C.syn checks it
 */
void Enemy_PropToy_SpawnTwinChain8(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *a2 = Scr2_03AB88;
    dk::jump(at_Obj_SpawnEnemy);
}
