#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_036974 = 0x036974u;

/* Enemy_DropCarrier_SpawnRabbitsC - spawner of a drop carrier that crosses at cruise $255, stops after
 * $780 ticks and then, after a random $3C0-$5DC0 wait and once the population is below 1, drops
 * white-rabbit hoppers (Spawn_AtOwnOfs)
 * In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
 * Out: script (a2) = Scr2_036974, then Obj_SpawnEnemy's results
 * Evidence: lea Scr2_036974,a2 / jmp Obj_SpawnEnemy; Scr2_036974: Enemy_DropCarrier_SpawnRabbits's
 *           opening (energy $100, score $12C, coroutine 1 $035B34, Op2_PlaceYOnScreen) with cruise $255
 *           and coroutine 0 $036B54; the wave spawn entries at $03469A and $0346A2 (x -$28 / $168, y
 *           $E06A, action byte $26 on top of the spawner long) name it; no corpus run reaches it
 *           (raw-stubs 2026-10-30); the contract is Enemy_DropCarrier_SpawnRabbits's (the same kind of
 *           call); 036968.syn checks it
 */
void Enemy_DropCarrier_SpawnRabbitsC(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_036974;
    dk::jump(at_Obj_SpawnEnemy);
}
