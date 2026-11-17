#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_036512 = 0x036512u;

/* Enemy_PuffPlant_SpawnE - puff plant (Scr2_036512) that waits $2D0 ticks, then every $1E0 ticks
 * puffs 10 puffs with the second puff script ($35714)
 * In:  x/y = spawn position (d4/d5)
 * Out: script = Scr2_036512, then Obj_SpawnEnemy's (d0 = 0 and the new object's blocks;
 *      d0 nonzero when the pool is short)
 * Evidence: lea Scr2_036512,a2 / jmp Obj_SpawnEnemy; trigger entries $034018, $034042 (Tbl_033F30); TAS f96261-f96325
 *           The prototype hands on a2 (measured as a clobber):
 *           dk::jump passes only the prototype's registers, and Obj_SpawnEnemy reads a2
 */
void Enemy_PuffPlant_SpawnE(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *script, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_036512;
    dk::jump(at_Obj_SpawnEnemy);
}
