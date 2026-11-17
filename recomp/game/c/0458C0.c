#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_0458CC = 0x0458CCu;

/* Enemy_VentTank_SpawnFastest - spawns a lab vent tank (body $42D74, floor height $9E, energy $300)
 * that cruises at $2AA, vents $F00 ticks between $780-tick rests
 * In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
 * Out: script (a2) = Scr2_0458CC, then Obj_SpawnEnemy's results
 * Evidence: lea Scr2_0458CC,a2 / jmp Obj_SpawnEnemy; Scr2_0458CC: score $C8, energy $300, x
 *           accel/cruise $2AA, coroutine 0 $458EE, Goto the vent tank body $42D74; the lab wave's spawn
 *           entry at $0417A0 (x $168, y $E070) names it; no corpus run reaches it (raw-stubs
 *           2026-10-30); the contract is Enemy_VentTank_SpawnFast's (the same kind of call); 0458C0.syn
 *           checks it
 */
void Enemy_VentTank_SpawnFastest(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_0458CC;
    dk::jump(at_Obj_SpawnEnemy);
}
