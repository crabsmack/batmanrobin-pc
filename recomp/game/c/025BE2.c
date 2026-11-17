#include "game.h"

/* the boss director's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_025BEE = 0x025BEEu;

/* Boss_Stage2_Spawn - spawns the stage 2 arena boss director Scr2_025BEE (boss class): it waits
 * until no enemy is left, loads the boss art through its a6 loader script, spawns the four
 * turrets and starts the fight
 * In:  x/y = where the director object starts
 * Out: script = Scr2_025BEE, then Obj_SpawnBoss: d0 = 0 and a6 = the boss object (Z clear and d0
 *      nonzero when the pool is short)
 * Evidence: lea Scr2_025BEE,a2 / jmp Obj_SpawnBoss; jmp'd to from Stage2_LiftArriveTop ($0241CC)
 *           when the lift reaches the top; TAS f32065 (one frame); Scr2_025BEE: Op2_WaitPopBelow 1,
 *           Op2_StartA6Script $32ED6, four Op2_SpawnChildOrWait (fields $1E-$24)
 */
void Boss_Stage2_Spawn(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_025BEE;
    dk::jump(at_Obj_SpawnBoss);   /* jmp: Obj_SpawnBoss returns to our caller */
}
