#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_03189E = 0x03189Eu;

/* Enemy_PipeCannon_Spawn3Way - spawns a pipe cannon: it slides up and down a hull pipe between map
 * y $C8 and $12C (coroutine 0) and three times, every $1E0 while on screen, fires a 3-way spread to
 * the left (Fx_SpawnAtOfsAngle at (-8, 0), headings $240/$200/$1C0, a4 $31930), then swaps target
 * and pauses $5A0
 * In:  x/y = the trigger entry's map position
 * Out: script = Scr2_03189E, then Obj_SpawnEnemy: Z set and a6 = the new cannon when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_03189E,a2 / jmp Obj_SpawnEnemy; trigger entry Tbl_0307C2 +$6E2 (drop byte
 *           $11); body Scr2_030416 (energy $110, moves in map space: Flight_AirshipLoadMapPos /
 *           StoreMapPos, frames by Enemy_PipeCannon_PipePhase); TAS f69585. The prototype gains &a2
 *           (measured a clobber: Trigger_Fire saves and restores a2 around its callback) so the jump
 *           hands the script on
 */
void Enemy_PipeCannon_Spawn3Way(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *script, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_03189E;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
