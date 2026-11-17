#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_03CB90 = 0x03CB90u;

/* Stage3_SpawnRevealDirector - loader-script native that spawns the forest-clearing reveal director
 * (boss class, Scr2_03CB90): boss music $A, the line-3 colour cycle, Stage3_RevealInit, 36 timed
 * Stage3_RevealBurst bursts, flashes and shakes while Stage3_RevealNextCell fills the rest, then
 * Boss_ForestRobot_Spawn
 * In:  -
 * Out: jumps to Obj_SpawnBoss with a2 = Scr2_03CB90 (d0 = 0 spawned; nonzero makes
 *      Op6_CallNativeRetry call it again next frame)
 * Evidence: `70BE 0003CB84` (Op6_CallNativeRetry) at Scr6_040054+$88, the clearing scene's loader;
 *           TAS f97887 (once); the picture assembling at f98400-f100800
 */
void Stage3_SpawnRevealDirector(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a0, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_03CB90;
    dk::jump(at_Obj_SpawnBoss);
}
