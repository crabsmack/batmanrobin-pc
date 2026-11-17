#include "game.h"

/* the facade scene camera's a2 script (ROM) */
static constexpr uint32_t Scr2_0487DA = 0x0487DAu;

/* Story_SpawnFacadeCamera - spawns the asylum facade scene's camera object (Scr2_0487DA): ends the
 * scene after $12C frames, four white flashes with big shakes at random gaps (the lightning), a
 * last flash, then it rises; its native Story_FacadeScroll scrolls both planes by its y.
 * In:  x, y = the spawn position (Obj_SpawnEnemy's d4/d5)
 * Out: script = Scr2_0487DA; Obj_SpawnEnemy's results (a6 = the object, d0 = 0 / -1)
 * Evidence: lea Scr2_0487DA,a2 / jmp Obj_SpawnEnemy; Scr2_0487DA: Scene_EndAfterDelay $12C, 4 x
 *           (Fade_FlashWhite3, Shake_StartBigIfIdle, random wait), Fade_FlashWhite6, JumpAnim $FF98
 *           $780; only caller Story_AsylumFacade; idle 3335 + 3925n
 */
void Story_SpawnFacadeCamera(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                             uint32_t *a3, uint32_t *a4, uint32_t a6)
{
    *script = Scr2_0487DA;
    dk::jump(at_Obj_SpawnEnemy);
}
