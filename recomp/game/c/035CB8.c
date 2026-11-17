#include "game.h"

/* the rabbit's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_035CC4 = 0x035CC4u;

/* Enemy_BombRabbit_SpawnDropIn - wave spawner of a bomb rabbit that drops in from above
 * (Scr2_035CC4: coroutine 0 = the fall op, then the walk animation; cruise x $1AA)
 * In:  x/y = spawn position (the wave entry)
 * Out: script = Scr2_035CC4, then Obj_SpawnEnemy's (d0 = 0 and a3/a4/a6 = the new rabbit's blocks;
 *      d0 nonzero when the pool is short)
 * Evidence: lea Scr2_035CC4,a2 / jmp Obj_SpawnEnemy; wave spawn entries $034E54-$034E74; TAS
 *           f82614-f115324 (3 spawns)
 */
void Enemy_BombRabbit_SpawnDropIn(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7,
                                  uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_035CC4;
    dk::jump(at_Obj_SpawnEnemy);
}
