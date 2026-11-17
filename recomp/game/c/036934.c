#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_036940 = 0x036940u;

/* Enemy_DropCarrier_SpawnRabbitsB - spawner of a drop carrier placed on the screen (Op2_PlaceOnScreen)
 * that crosses at cruise $255 and, after $780 ticks, drops white-rabbit hoppers (Spawn_AtOwnOfs)
 * whenever the population is below $F, $5A0 ticks apart
 * In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
 * Out: script (a2) = Scr2_036940, then Obj_SpawnEnemy's results
 * Evidence: lea Scr2_036940,a2 / jmp Obj_SpawnEnemy; Scr2_036940: Enemy_DropCarrier_SpawnRabbits's
 *           opening (energy $100, score $12C, coroutine 1 $035B34) with cruise $255 and coroutine 0
 *           $036A2C (Op2_SwitchRandom, Op2_WaitPopBelowYield0 $F, Op2_CallNative Spawn_AtOwnOfs); the
 *           wave spawn entry at $03492E (x 0) names it; no corpus run reaches it (raw-stubs
 *           2026-10-30); the contract is Enemy_DropCarrier_SpawnRabbits's (the same kind of call);
 *           036934.syn checks it
 */
void Enemy_DropCarrier_SpawnRabbitsB(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_036940;
    dk::jump(at_Obj_SpawnEnemy);
}
