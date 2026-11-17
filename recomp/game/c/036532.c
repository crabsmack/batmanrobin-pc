#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_03653E = 0x03653Eu;

/* Enemy_PuffPlant_SpawnF - puff plant (Scr2_03653E) that puffs 10 puffs every $1E0 ticks with the
 * second puff script ($35714)
 * In:  x/y = spawn position (d4/d5)
 * Out: script = Scr2_03653E, then Obj_SpawnEnemy's (d0 = 0 and the new object's blocks;
 *      d0 nonzero when the pool is short)
 * Evidence: lea Scr2_03653E,a2 / jmp Obj_SpawnEnemy; trigger entries $034034, $034050 (Tbl_033F30); TAS f96261-f96325
 *           The prototype hands on a2 (measured as a clobber):
 *           dk::jump passes only the prototype's registers, and Obj_SpawnEnemy reads a2
 */
void Enemy_PuffPlant_SpawnF(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *script, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_03653E;
    dk::jump(at_Obj_SpawnEnemy);
}
