#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02D76E = 0x02D76Eu;

/* Enemy_Biplane_SpawnWeave - wave spawner: a red biplane (Scr2_02D76E) that weaves through a long
 * course of 120-px legs with rising speed ($400-$600), then a +-40 px zigzag and a 600-px exit, and
 * loops
 * In:  x/y = where the trigger or wave entry places it
 * Out: script = Scr2_02D76E, then Obj_SpawnEnemy: Z set and a6 = the new object when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_02D76E,a2 / jmp Obj_SpawnEnemy; Scr5_023C78 +$2F0/+$328/+$350 (origin entries,
 *           group action 1); TAS 36399-42273 (30 frames)
 */
void Enemy_Biplane_SpawnWeave(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_02D76E;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
