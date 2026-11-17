#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_045C08 = 0x045C08u;

/* Enemy_GunTank_SpawnHeavy - spawns a lab gun tank (body $42FEC, floor height $92) with energy $300 and
 * score $190 that cruises at $22A
 * In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
 * Out: script (a2) = Scr2_045C08, then Obj_SpawnEnemy's results
 * Evidence: lea Scr2_045C08,a2 / jmp Obj_SpawnEnemy; Scr2_045C08: score $190, energy $300, x
 *           accel/cruise $22A, coroutines 0 $45C2E and 1 $45C46, Goto the gun tank body $42FEC; the lab
 *           wave's spawn entry at $0416BC (x $154, y $E070) names it; no corpus run reaches it
 *           (raw-stubs 2026-10-30); the contract is Enemy_GunTank_SpawnTough's (the same kind of call);
 *           045BFC.syn checks it
 */
void Enemy_GunTank_SpawnHeavy(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_045C08;
    dk::jump(at_Obj_SpawnEnemy);
}
