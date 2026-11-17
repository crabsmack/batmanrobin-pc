#include "game.h"

/* the rabbit's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_035CFE = 0x035CFEu;

/* Enemy_BombRabbit_SpawnHop - spawner of a bomb rabbit that hops in (Scr2_035CFE: the jump op
 * $46/$140, then the walk at cruise $1AA; its small touch box lights the fuse)
 * In:  x/y = spawn position (a wave entry, or the rabbit pot's Spawn_AtOwnOfs)
 * Out: script = Scr2_035CFE, then Obj_SpawnEnemy's (d0 = 0 and a3/a4/a6 = the new rabbit's blocks;
 *      d0 nonzero when the pool is short)
 * Evidence: lea Scr2_035CFE,a2 / jmp Obj_SpawnEnemy; wave entries ($034E7C, $035062-$0350CC) and
 *           the rabbit pot's spawns ($03674C, $036782); TAS f82766-f114152 (13 spawns)
 */
void Enemy_BombRabbit_SpawnHop(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7,
                               uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_035CFE;
    dk::jump(at_Obj_SpawnEnemy);
}
