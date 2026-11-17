#include "game.h"

/* the object's a2 script (ROM) */
static constexpr uint32_t Scr2_049E3A = 0x049E3Au;

/* ClockworkLogo_Spawn - spawns the Clockwork Tortoise logo object Scr2_049E3A: ends the scene after 230
 * frames, moves the picture vertically from y -$84 (cruise, then a hop and bounce in coroutine 0) and
 * every 2 frames cycles line 3's colours
 * In:  x, y = the spawn position (Obj_SpawnEnemy's d4/d5)
 * Out: script = Scr2_049E3A; Obj_SpawnEnemy's results (a6 = the object, d0 = 0 / -1)
 * Evidence: lea Scr2_049E3A,a2 / jmp Obj_SpawnEnemy; Scr2_049E3A: Scene_EndAfterDelay 0/$E6,
 *           Op2_PlaceOnScreen 0/$FF7C, Op2_SetCruiseY, coroutine $49E72, timer 3 $20 ->
 *           Op2_CallNativeRel $49EA4, then $49E98 every frame; idle f3678
 */
void ClockworkLogo_Spawn(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                         uint32_t *a3, uint32_t *a4, uint32_t a6)
{
    *script = Scr2_049E3A;
    dk::jump(at_Obj_SpawnEnemy);
}
