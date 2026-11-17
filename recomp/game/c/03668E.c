#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_03669A = 0x03669Au;

/* Enemy_RabbitPot_SpawnB - rabbit pot (Scr2_03669A) that slides in, puffs 30 shots, then lets out
 * two hopping bomb rabbits ($780 ticks apart), and repeats
 * In:  x/y = spawn position (d4/d5)
 * Out: script = Scr2_03669A, then Obj_SpawnEnemy's (d0 = 0 and the new object's blocks;
 *      d0 nonzero when the pool is short)
 * Evidence: lea Scr2_03669A,a2 / jmp Obj_SpawnEnemy; wave entries $0348C6/$0348CE; TAS f112995
 */
void Enemy_RabbitPot_SpawnB(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_03669A;
    dk::jump(at_Obj_SpawnEnemy);
}
