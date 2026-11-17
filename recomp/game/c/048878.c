#include "game.h"

/* the monitors scene object's a2 script (ROM) */
static constexpr uint32_t Scr2_048884 = 0x048884u;

/* Story_SpawnMonitorsCamera - spawns the security monitors scene's object (Scr2_048884): pans down
 * with Story_MonitorsScroll as its slot-2 native, bounces, and after $B40 ticks starts a shake and
 * switches to Story_MonitorsAlarmScroll (the red alarm flashing) with six random shakes; a
 * coroutine runs the a6 loader scripts that animate the screens.
 * In:  x, y = the spawn position (Obj_SpawnEnemy's d4/d5)
 * Out: script = Scr2_048884; Obj_SpawnEnemy's results (a6 = the object, d0 = 0 / -1)
 * Evidence: lea Scr2_048884,a2 / jmp Obj_SpawnEnemy; Scr2_048884: StartCoroutine2Rel to $0489CC
 *           then $048998, CruiseDown, JumpAnim $5C $3C0, Op2_StartA6Script $047F6E..., Scene_EndAfterDelay
 *           $78; only caller Story_Monitors; idle 2066 + 3925n
 */
void Story_SpawnMonitorsCamera(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                               uint32_t *a3, uint32_t *a4, uint32_t a6)
{
    *script = Scr2_048884;
    dk::jump(at_Obj_SpawnEnemy);
}
