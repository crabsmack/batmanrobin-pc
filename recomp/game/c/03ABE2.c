#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_03ABEE = 0x03ABEEu;

/* Enemy_PropToy_SpawnChain5RightC - spawner of a propeller toy entering from the right (H-flipped) with
 * the 5-link chain ToyChain_MakeHanging5E, course coroutine $03AC06 at cruise $1AA, body $037E26
 * In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
 * Out: a2 (a2) = Scr2_03ABEE, then Obj_SpawnEnemy's results
 * Evidence: lea Scr2_03ABEE,a2 / jmp Obj_SpawnEnemy; Scr2_03ABEE: Op2_SpawnChild $2A $03AEBA,
 *           Op2_ModSprAttr $F008, coroutine 0 $03AC06, Goto $037E26; the wave spawn entries at $035106,
 *           $03510E, $035116 and $03511E (x -$28 / $168) name it; no corpus run reaches it (raw-stubs
 *           2026-10-30); the contract is Enemy_PropToy_SpawnChain6Right's (the same kind of call);
 *           03ABE2.syn checks it
 */
void Enemy_PropToy_SpawnChain5RightC(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *a2 = Scr2_03ABEE;
    dk::jump(at_Obj_SpawnEnemy);
}
