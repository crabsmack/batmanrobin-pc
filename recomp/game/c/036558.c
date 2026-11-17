#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_036564 = 0x036564u;

/* Enemy_PuffPlant_SpawnG - puff plant (Scr2_036564) whose loop is picked at random
 * (Op2_SwitchRandom): mixes of the three puff scripts in runs of 4-25 puffs
 * In:  x/y = spawn position (d4/d5)
 * Out: script = Scr2_036564, then Obj_SpawnEnemy's (d0 = 0 and the new object's blocks;
 *      d0 nonzero when the pool is short)
 * Evidence: lea Scr2_036564,a2 / jmp Obj_SpawnEnemy; trigger entries $03407E-$0340D2 (Tbl_033F30); TAS f96435-f96681 (6 spawns)
 *           The prototype hands on a2 (measured as a clobber):
 *           dk::jump passes only the prototype's registers, and Obj_SpawnEnemy reads a2
 */
void Enemy_PuffPlant_SpawnG(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *script, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_036564;
    dk::jump(at_Obj_SpawnEnemy);
}
