#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_0443E8 = 0x0443E8u;

/* Enemy_HoverPod_Spawn7G - spawns a hall hover pod at a random speed ($66-$CC) on course 7 ($447DE:
 * waypoint gosub $45336 every $3C0 ticks) with attack G ($447EC: turn rate $555, five-shot fans),
 * cruise $1AA on both axes
 * In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
 * Out: script (a2) = Scr2_0443E8, then Obj_SpawnEnemy's results
 * Evidence: lea Scr2_0443E8,a2 / jmp Obj_SpawnEnemy; Scr2_0443E8: score $1F4, energy $1C0/$1C0,
 *           accel/cruise $1AA, coroutine 0 $447DE, coroutine 1 $447EC, JitterPos $12C $28, MoveY -$96,
 *           Goto the hover pod body $42152 (as Enemy_HoverPod_Spawn1A); the wave spawn entry at $041726
 *           (x -$C, y $A0) names it; no corpus run reaches it (raw-stubs 2026-10-30); the contract is
 *           Enemy_HoverPod_Spawn1A's (the same kind of call); 0443DC.syn checks it
 */
void Enemy_HoverPod_Spawn7G(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_0443E8;
    dk::jump(at_Obj_SpawnEnemy);
}
