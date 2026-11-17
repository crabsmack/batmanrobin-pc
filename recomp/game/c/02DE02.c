#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02DE0E = 0x02DE0Eu;

/* Enemy_Biplane_SpawnZigzagB - wave spawner: a red biplane (Scr2_02DE0E) that flies 166 px in and
 * 56 px down, zigzags +-10 px downward while speeding up, then +-40 px with legs climbing farther
 * each time, then loops through a long zigzag
 * In:  x/y = where the trigger or wave entry places it
 * Out: script = Scr2_02DE0E, then Obj_SpawnEnemy: Z set and a6 = the new object when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_02DE0E,a2 / jmp Obj_SpawnEnemy; Scr5_023C78 +$2A6 (origin entry, group action
 *           1); a never-run spawner follows inside the span at $2DF4C (lea Scr2_02DF58 / jmp
 *           Obj_SpawnEnemy, raw); fuzz runs only
 */
void Enemy_Biplane_SpawnZigzagB(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_02DE0E;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
