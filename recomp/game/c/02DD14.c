#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02DD20 = 0x02DD20u;

/* Enemy_Biplane_SpawnNarrowUp - wave spawner: a red biplane (Scr2_02DD20) on the narrow course
 * mirrored upward: 160 px in and 48 px up, then a +-20 px zigzag drifting up at rising speed, then
 * off left
 * In:  x/y = where the trigger or wave entry places it
 * Out: script = Scr2_02DD20, then Obj_SpawnEnemy: Z set and a6 = the new object when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_02DD20,a2 / jmp Obj_SpawnEnemy; Scr5_023C78 +$248 (x $154, y $20F8), spawned
 *           right after Enemy_Biplane_SpawnNarrowDown; TAS 45022-45113
 */
void Enemy_Biplane_SpawnNarrowUp(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_02DD20;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
