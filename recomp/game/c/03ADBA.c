#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_03ADC6 = 0x03ADC6u;

/* Enemy_PropToy_SpawnTwinChain6 - spawner of a propeller toy entering from the right (H-flipped) with
 * two 6-link chains (children $2A ToyChain_MakeHanging6C, $2C ToyChain_MakeHanging6D), course coroutine
 * $03ADEA, body $037E26
 * In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
 * Out: a2 (a2) = Scr2_03ADC6, then Obj_SpawnEnemy's results
 * Evidence: lea Scr2_03ADC6,a2 / jmp Obj_SpawnEnemy; Scr2_03ADC6: SetAnim, Op2_SpawnChild $2A $03B064,
 *           SetAnim, Op2_SpawnChild $2C $03B092, Op2_ModSprAttr $F008, coroutine 0 $03ADEA; the wave
 *           spawn entry at $035126 (x $168, y $E002) names it; no corpus run reaches it (raw-stubs
 *           2026-10-30); the contract is Enemy_PropToy_SpawnChain6Right's (the same kind of call);
 *           03ADBA.syn checks it
 */
void Enemy_PropToy_SpawnTwinChain6(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *a2 = Scr2_03ADC6;
    dk::jump(at_Obj_SpawnEnemy);
}
