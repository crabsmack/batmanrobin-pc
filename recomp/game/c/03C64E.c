#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_03C65A = 0x03C65Au;

/* Boss_ForestRobot_Spawn - spawner of the forest-clearing boss, a giant copper robot with a jowly
 * snouted face and a claw arm (Scr2_03C65A: 5000 points, energy $1200/$1B00, BossMeter_Show, drops
 * in from above at cruise y $A00, swaps its player target every $780 ticks)
 * In:  d4/d5 = position (Spawn_AtCameraOfs: camera + (0, -$64))
 * Out: jumps to Obj_SpawnBoss with a2 = Scr2_03C65A
 * Evidence: dc.l $03C64E after Spawn_AtCameraOfs at the end of the reveal director Scr2_03CB90
 *           ($03CC70); TAS f100733; the robot at screen (38,11) in RAM f101100 and on screen at
 *           f101300/f101400 (BOSS TIME)
 *           a2 an output (measured as a clobber): Obj_SpawnBoss, jumped to, takes the script in it
 */
void Boss_ForestRobot_Spawn(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script)
{
    *script = Scr2_03C65A;
    dk::jump(at_Obj_SpawnBoss);
}
