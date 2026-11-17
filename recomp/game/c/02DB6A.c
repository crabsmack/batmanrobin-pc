#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02DB76 = 0x02DB76u;

/* Enemy_Biplane_SpawnWideUp - wave spawner: the mirror of Enemy_Biplane_SpawnWideDown (Scr2_02DB76:
 * 48 px up, the wide zigzag drifting up); the script's tail $2DC42 is the narrow course entered by
 * Enemy_Biplane_SpawnNarrowDown
 * In:  x/y = where the trigger or wave entry places it
 * Out: script = Scr2_02DB76, then Obj_SpawnEnemy: Z set and a6 = the new object when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_02DB76,a2 / jmp Obj_SpawnEnemy; Scr5_023C78 +$198/+$206 (x $154, y $20F8); TAS
 *           37975-38055
 */
void Enemy_Biplane_SpawnWideUp(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_02DB76;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
