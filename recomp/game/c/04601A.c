#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_046026 = 0x046026u;

/* Enemy_GunTank_SpawnToughSlow - spawns a tough lab gun tank (energy $200, score $190) at a random
 * speed ($33-$E6) with cruise $155
 * In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
 * Out: script (a2) = Scr2_046026, then Obj_SpawnEnemy's results
 * Evidence: lea Scr2_046026,a2 / jmp Obj_SpawnEnemy; Scr2_046026: as Enemy_GunTank_SpawnTough's script
 *           with x accel/cruise $155 and coroutines 0 $4604E / 1 $4605C; the lab wave's spawn entry at
 *           $04175E (x $17C, y $E070) names it; no corpus run reaches it (raw-stubs 2026-10-30); the
 *           contract is Enemy_GunTank_SpawnTough's (the same kind of call); 04601A.syn checks it
 */
void Enemy_GunTank_SpawnToughSlow(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_046026;
    dk::jump(at_Obj_SpawnEnemy);
}
