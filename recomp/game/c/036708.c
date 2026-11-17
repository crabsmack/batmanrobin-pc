#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_036714 = 0x036714u;

/* Enemy_RabbitPot_SpawnD - rabbit pot (Scr2_036714) already in place that only releases bomb
 * rabbits: random sequences of three $3C0 ticks apart, then a $B40 pause
 * In:  x/y = spawn position (d4/d5)
 * Out: script = Scr2_036714, then Obj_SpawnEnemy's (d0 = 0 and the new object's blocks;
 *      d0 nonzero when the pool is short)
 * Evidence: lea Scr2_036714,a2 / jmp Obj_SpawnEnemy; a computed-call target in the corpus; TAS f111764
 *           The prototype hands on a2 (measured as a clobber):
 *           dk::jump passes only the prototype's registers, and Obj_SpawnEnemy reads a2
 */
void Enemy_RabbitPot_SpawnD(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *script, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_036714;
    dk::jump(at_Obj_SpawnEnemy);
}
