#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_03195C = 0x03195Cu;

/* Enemy_PipeCannon_Spawn3Then5Way - spawns a pipe cannon (the patrol of Enemy_PipeCannon_Spawn3Way)
 * whose volleys alternate a 3-way spread ($240/$200/$1C0) and, $3C0 later, a 5-way spread
 * ($260/$230/$200/$1D0/$1A0)
 * In:  x/y = the trigger entry's map position
 * Out: script = Scr2_03195C, then Obj_SpawnEnemy: Z set and a6 = the new cannon when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_03195C,a2 / jmp Obj_SpawnEnemy; trigger entry Tbl_0307C2 +$6F0 (drop byte
 *           $17); coroutine 1 $3198C; body Scr2_030416; TAS f72177. The prototype gains &a2
 *           (measured a clobber: Trigger_Fire saves and restores a2 around its callback) so the jump
 *           hands the script on
 */
void Enemy_PipeCannon_Spawn3Then5Way(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *script, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_03195C;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
