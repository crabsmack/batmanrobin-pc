#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_036448 = 0x036448u;

/* Enemy_PuffPlant_SpawnA - spawner of a puff plant (Scr2_036448, body $0355D4): every $168 ticks
 * it opens and puffs 10 rising puffs (puff script $356EE)
 * In:  x/y = spawn position (d4/d5)
 * Out: script = Scr2_036448, then Obj_SpawnEnemy's (d0 = 0 and the new object's blocks;
 *      d0 nonzero when the pool is short)
 * Evidence: lea Scr2_036448,a2 / jmp Obj_SpawnEnemy; trigger entries $033F60-$033F98 in Tbl_033F30; TAS f92044-f92084
 *           The prototype hands on a2 (measured as a clobber):
 *           dk::jump passes only the prototype's registers, and Obj_SpawnEnemy reads a2
 */
void Enemy_PuffPlant_SpawnA(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *script, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_036448;
    dk::jump(at_Obj_SpawnEnemy);
}
