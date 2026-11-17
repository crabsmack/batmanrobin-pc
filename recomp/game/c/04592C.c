#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_045938 = 0x045938u;

/* Enemy_VentTank_SpawnRandomSpeed - spawns a lab vent tank (body $42D74, floor height $9E, energy $300)
 * at a random speed ($33-$E6), cruise $D5, its first vent after $3C0 ticks
 * In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
 * Out: script (a2) = Scr2_045938, then Obj_SpawnEnemy's results
 * Evidence: lea Scr2_045938,a2 / jmp Obj_SpawnEnemy; Scr2_045938: speed $33-$E6, score $C8, energy
 *           $300, x accel/cruise $D5, coroutine 0 $4595C (gosub $42DEA), Goto the vent tank body
 *           $42D74; the lab wave's spawn entries at $0417A8 and $0417B0 (x -$28 / $168, y $E070) name
 *           it; no corpus run reaches it (raw-stubs 2026-10-30); the contract is
 *           Enemy_VentTank_SpawnFast's (the same kind of call); 04592C.syn checks it
 */
void Enemy_VentTank_SpawnRandomSpeed(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_045938;
    dk::jump(at_Obj_SpawnEnemy);
}
