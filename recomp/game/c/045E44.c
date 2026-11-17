#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_045E50 = 0x045E50u;

/* Enemy_GunTank_SpawnLow - spawns a lab gun tank as Enemy_GunTank_Spawn (cruise $80, energy $100,
 * coroutine 0 $45DEC) but on the lower floor line $A3 and with its own fire coroutine $45E9E
 * In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
 * Out: script (a2) = Scr2_045E50, then Obj_SpawnEnemy's results
 * Evidence: lea Scr2_045E50,a2 / jmp Obj_SpawnEnemy; Scr2_045E50: Enemy_GunTank_Spawn's script with
 *           PlaceYOnScreen $A3 (not $92) and coroutine 1 $45E9E; the wave spawn entries at $041402 and
 *           $04140A (x -$28 / $168, y $E070) name it; no corpus run reaches it (raw-stubs 2026-10-30);
 *           the contract is Enemy_GunTank_SpawnTough's (the same kind of call); 045E44.syn checks it
 */
void Enemy_GunTank_SpawnLow(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_045E50;
    dk::jump(at_Obj_SpawnEnemy);
}
