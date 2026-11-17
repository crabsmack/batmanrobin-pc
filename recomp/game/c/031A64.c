#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_031A70 = 0x031A70u;

/* Enemy_PipeCannon_Spawn5Way - spawns a pipe cannon (the patrol of Enemy_PipeCannon_Spawn3Way) that
 * fires only 5-way spreads ($260/$230/$200/$1D0/$1A0), three per cycle $5A0 apart
 * In:  x/y = the trigger entry's map position
 * Out: script = Scr2_031A70, then Obj_SpawnEnemy: Z set and a6 = the new cannon when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_031A70,a2 / jmp Obj_SpawnEnemy; trigger entry Tbl_0307C2 +$6FE (drop byte
 *           $1D); coroutine 1 $31AA0; body Scr2_030416; TAS f74442. The prototype gains &a2
 *           (measured a clobber: Trigger_Fire saves and restores a2 around its callback) so the jump
 *           hands the script on
 */
void Enemy_PipeCannon_Spawn5Way(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *script, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_031A70;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
