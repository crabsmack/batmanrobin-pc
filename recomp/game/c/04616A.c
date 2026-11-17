#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_046176 = 0x046176u;

/* Enemy_BombBot_SpawnHallNoDrop - spawns a hall bomb bot (the capsule robot, body $0432E2) like
 * Enemy_BombBot_SpawnHall but without its drop-init gosub: rolls in at cruise
 * x $1AA on the floor line y $B6 with a fuse (timer 0) of $B40 + random [0,
 * $B40)
 * In:  x/y (d4/d5) = spawn position (wave spawn entry: x $154 or -$14)
 * Out: script (a2) = Scr2_046176, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_046176,a2 / jmp Obj_SpawnEnemy; Scr2_046176: IncPopulation, score $C8, energy $100,
 *           SetCruiseX $1AA, PlaceYOnScreen $B6, SetTimer0Rand $B40 $B40, Goto $0432E2 (anim uses
 *           Enemy_BombBot_FrameByCameraX); Scr2_046140 (Enemy_BombBot_SpawnHall) is the same after a Gosub;
 *           spawn entries at $041174/$04117C; TAS 116192-118360 (4 spawns), the robot at screen x 241 in a
 *           shot of f118440
 */
void Enemy_BombBot_SpawnHallNoDrop(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                                   uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_046176;
    dk::jump(at_Obj_SpawnEnemy);
}
