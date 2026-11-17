#include "game.h"

/* the rabbit's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_035D38 = 0x035D38u;

/* Enemy_BombRabbit_SpawnHopPop4 - Enemy_BombRabbit_SpawnHop with a population weight of 4
 * (Scr2_035D38: Op2_AddPopulation 4), so the waves hold back while it lives
 * In:  x/y = spawn position
 * Out: script = Scr2_035D38, then Obj_SpawnEnemy's (d0 = 0 and a3/a4/a6 = the new rabbit's blocks;
 *      d0 nonzero when the pool is short)
 * Evidence: lea Scr2_035D38,a2 / jmp Obj_SpawnEnemy; dc.l at $03891C (a wave entry); level-skip
 *           scenario f10651-f10831. The prototype hands on a2 (and a3/a4/a6, as SpawnHop's):
 *           dk::jump passes only the prototype's registers, and Obj_SpawnEnemy reads a2
 */
void Enemy_BombRabbit_SpawnHopPop4(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7,
                                   uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_035D38;
    dk::jump(at_Obj_SpawnEnemy);
}
