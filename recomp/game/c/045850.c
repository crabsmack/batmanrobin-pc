#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_04585C = 0x04585Cu;

/* Enemy_VentTank_SpawnFaster - spawns a lab vent tank (body $42D74, floor height $9E, energy $300) that
 * cruises at $155 and repeats three vent rounds ($780 ticks) then a rest
 * In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
 * Out: script (a2) = Scr2_04585C, then Obj_SpawnEnemy's results
 * Evidence: lea Scr2_04585C,a2 / jmp Obj_SpawnEnemy; Scr2_04585C: score $C8, energy $300, x
 *           accel/cruise $155, coroutine 0 $4587E (gosub $41D50 chase, waits $780, counter 3), Goto the
 *           vent tank body $42D74; the lab wave's spawn entries at $0416B4 and $0416F0 (x $154 / $A0, y
 *           $E070) name it; no corpus run reaches it (raw-stubs 2026-10-30); the contract is
 *           Enemy_VentTank_SpawnFast's (the same kind of call); 045850.syn checks it
 */
void Enemy_VentTank_SpawnFaster(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_04585C;
    dk::jump(at_Obj_SpawnEnemy);
}
