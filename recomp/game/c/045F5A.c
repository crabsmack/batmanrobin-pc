#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_045F66 = 0x045F66u;

/* Enemy_GunTank_SpawnToughFast - spawns a tough lab gun tank (energy $200, score $190) at a random
 * speed ($33-$E6) with cruise $22A, counted in the population
 * In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
 * Out: script (a2) = Scr2_045F66, then Obj_SpawnEnemy's results
 * Evidence: lea Scr2_045F66,a2 / jmp Obj_SpawnEnemy; Scr2_045F66: Op2_IncPopulation, speed $33-$E6,
 *           score $190, energy $200, x accel/cruise $22A, coroutines 0 $45FC6 and 1 $45FDE, Goto the
 *           gun tank body $42FEC; the lab wave's spawn entry at $041756 (x $17C, y $E070) names it; no
 *           corpus run reaches it (raw-stubs 2026-10-30); the contract is Enemy_GunTank_SpawnTough's
 *           (the same kind of call); 045F5A.syn checks it
 */
void Enemy_GunTank_SpawnToughFast(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_045F66;
    dk::jump(at_Obj_SpawnEnemy);
}
