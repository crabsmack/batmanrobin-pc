#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02DA9E = 0x02DA9Eu;

/* Enemy_Biplane_SpawnWideDown - wave spawner: a red biplane (Scr2_02DA9E) that flies 160 px in and
 * 48 px down, then zigzags +-40 px drifting down, then +-60 px drifting back up, and leaves left
 * In:  x/y = where the trigger or wave entry places it
 * Out: script = Scr2_02DA9E, then Obj_SpawnEnemy: Z set and a6 = the new object when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_02DA9E,a2 / jmp Obj_SpawnEnemy; Scr5_023C78 +$190/+$1FE (x $154, y $2094),
 *           first of a 4-plane formation with WideUp, ZigzagDown and ZigzagUp; TAS 41152-41232
 */
void Enemy_Biplane_SpawnWideDown(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_02DA9E;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
