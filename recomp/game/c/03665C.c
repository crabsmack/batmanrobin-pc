#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_036668 = 0x036668u;

/* Enemy_RabbitPot_SpawnA - spawner of the rabbit pot (Scr2_036668: energy $180, score $12C): it
 * slides in from the right to its screen x and puffs bursts of 20 shots forever
 * In:  x/y = spawn position (d4/d5)
 * Out: script = Scr2_036668, then Obj_SpawnEnemy's (d0 = 0 and the new object's blocks;
 *      d0 nonzero when the pool is short)
 * Evidence: lea Scr2_036668,a2 / jmp Obj_SpawnEnemy; wave spawn entries $0347EC/$0347F4/$0348B6/$0348BE; TAS f111166-f112916
 */
void Enemy_RabbitPot_SpawnA(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_036668;
    dk::jump(at_Obj_SpawnEnemy);
}
