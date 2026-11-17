#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02DA82 = 0x02DA82u;

/* Enemy_Biplane_SpawnNarrowDown - wave spawner: a red biplane (Scr2_02DA82) that jumps into the
 * second half of Enemy_Biplane_SpawnWideUp's script ($2DC42): 160 px in and 48 px down, then a
 * narrow +-20 px zigzag drifting 6 px per leg at rising speed, then off left
 * In:  x/y = where the trigger or wave entry places it
 * Out: script = Scr2_02DA82, then Obj_SpawnEnemy: Z set and a6 = the new object when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_02DA82,a2 / jmp Obj_SpawnEnemy; Scr2_02DA82 is 16 bytes (vector 0 = $2DC42);
 *           Scr5_023C78 +$240 (x $154, y $2094), paired with Enemy_Biplane_SpawnNarrowUp; TAS
 *           45013-45103
 */
void Enemy_Biplane_SpawnNarrowDown(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_02DA82;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
