#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_031242 = 0x031242u;

/* Enemy_HullTurret_SpawnSpinning - spawns a hull gun turret that keeps spinning (coroutine 0 =
 * Enemy_HullTurret_SpinStep: 45-degree steps at turn rate $1800) and, after a random wait, fires 5
 * shots $28 apart along its current heading, then swaps target
 * In:  x/y = the trigger entry's map position
 * Out: script = Scr2_031242, then Obj_SpawnEnemy: Z set and a6 = the new turret when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_031242,a2 / jmp Obj_SpawnEnemy; 3 trigger entries in Tbl_0307C2 (one with drop
 *           byte $11); the same body $2F98C; TAS 68185-73727 (2 spawns). The prototype gains &a2
 *           (measured a clobber: Trigger_Fire saves and restores a2 around its callback) so the jump
 *           hands the script on
 */
void Enemy_HullTurret_SpawnSpinning(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *script, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_031242;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
