#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_039C50 = 0x039C50u;

/* Enemy_GiftBox_SpawnStay10C - spawner of a gift box (box body Scr2_037C02) that stays put, opens after
 * $1E0+$1E0 ticks into a 10-figure chain (ToyChain_MakeBox10C), then hops ($1E/$F0) three times at
 * random $3C0-$780 intervals and once high ($46/$168), repeating
 * In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
 * Out: a2 (a2) = Scr2_039C50, then Obj_SpawnEnemy's results
 * Evidence: lea Scr2_039C50,a2 / jmp Obj_SpawnEnemy; Scr2_039C50: Enemy_GiftBox_SpawnRoamWide10's
 *           opening without Op2_AddPopulation (coroutine 0 $039C62: SpawnChild $2A $03A326,
 *           StartChildCoroutine3Rel/0Rel, JumpAnim), Goto $037C02; the wave spawn entry at $03490E (x
 *           $14A, y $E098) names it; no corpus run reaches it (raw-stubs 2026-10-30); the contract is
 *           Enemy_GiftBox_SpawnRoamWide10's (the same kind of call); 039C44.syn checks it
 */
void Enemy_GiftBox_SpawnStay10C(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *a2 = Scr2_039C50;
    dk::jump(at_Obj_SpawnEnemy);
}
