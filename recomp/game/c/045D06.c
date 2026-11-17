#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_045D12 = 0x045D12u;

/* Enemy_GunTank_SpawnToughMid - spawns a tough lab gun tank (body $42FEC, floor height $92, energy
 * $200, score $190) at a fixed speed with cruise $1AA
 * In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
 * Out: script (a2) = Scr2_045D12, then Obj_SpawnEnemy's results
 * Evidence: lea Scr2_045D12,a2 / jmp Obj_SpawnEnemy; Scr2_045D12: score $190, energy $200, x
 *           accel/cruise $1AA, coroutines 0 $45D6A and 1 $45D82, Goto the gun tank body $42FEC; the lab
 *           wave's spawn entry at $04173E (x $17C, y $E070) names it; no corpus run reaches it
 *           (raw-stubs 2026-10-30); the contract is Enemy_GunTank_SpawnTough's (the same kind of call);
 *           045D06.syn checks it
 */
void Enemy_GunTank_SpawnToughMid(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_045D12;
    dk::jump(at_Obj_SpawnEnemy);
}
