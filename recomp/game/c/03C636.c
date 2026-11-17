#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_03C642 = 0x03C642u;

/* Enemy_ArenaDebris_SpawnBounceAtX - spawner of a piece of arena debris that bounces twice on the floor
 * (coroutine 0 $03B66A) as Enemy_ArenaDebris_SpawnBounce but at the wave entry's x (no Op2_JitterPos);
 * body $03B5EC
 * In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
 * Out: a2 (a2) = Scr2_03C642, then Obj_SpawnEnemy's results
 * Evidence: lea Scr2_03C642,a2 / jmp Obj_SpawnEnemy; Scr2_03C642: SetSpeed $100, StartCoroutine0Rel
 *           $03B66A, Goto $03B5EC; the wave spawn entries at $03C54E, $03C56C, $03C596 and $03C5B4 (x
 *           $A0, y $E070) in the cat-head arena's waves name it; no corpus run reaches it (raw-stubs
 *           2026-10-30); the contract is Enemy_ArenaDebris_SpawnBounce's (the same kind of call);
 *           03C636.syn checks it
 */
void Enemy_ArenaDebris_SpawnBounceAtX(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *a2 = Scr2_03C642;
    dk::jump(at_Obj_SpawnEnemy);
}
