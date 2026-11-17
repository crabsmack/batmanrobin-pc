#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02D98A = 0x02D98Au;

/* Enemy_Biplane_SpawnZigzagUp - wave spawner: the mirror of Enemy_Biplane_SpawnZigzagDown
 * (Scr2_02D98A): the zigzag legs climb 4 px each
 * In:  x/y = where the trigger or wave entry places it
 * Out: script = Scr2_02D98A, then Obj_SpawnEnemy: Z set and a6 = the new object when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_02D98A,a2 / jmp Obj_SpawnEnemy; Scr5_023C78 +$1A8/+$216/+$27E; the two scripts
 *           differ only in the sign of the per-leg y words (4 / -4); TAS 47934-66212
 */
void Enemy_Biplane_SpawnZigzagUp(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_02D98A;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
