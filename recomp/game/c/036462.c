#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_03646E = 0x03646Eu;

/* Enemy_PuffPlant_SpawnB - puff plant (Scr2_03646E) that first waits $F0 ticks, then every $168
 * ticks puffs 10 puffs (script $356EE)
 * In:  x/y = spawn position (d4/d5)
 * Out: script = Scr2_03646E, then Obj_SpawnEnemy's (d0 = 0 and the new object's blocks;
 *      d0 nonzero when the pool is short)
 * Evidence: lea Scr2_03646E,a2 / jmp Obj_SpawnEnemy; trigger entry $033FA6 (Tbl_033F30); TAS f92084
 *           The prototype hands on a2 (measured as a clobber):
 *           dk::jump passes only the prototype's registers, and Obj_SpawnEnemy reads a2
 */
void Enemy_PuffPlant_SpawnB(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *script, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_03646E;
    dk::jump(at_Obj_SpawnEnemy);
}
