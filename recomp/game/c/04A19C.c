#include "game.h"

/* the object's a2 script (ROM) */
static constexpr uint32_t Scr2_04A1A8 = 0x04A1A8u;

/* BatmobileIntro_SpawnAnimator - spawns the Batmobile scene's object Scr2_04A1A8: ends the scene after
 * 120 frames and runs the native $4A1C8 each frame (rotates 4 colour triples of palette line 0 every 4
 * frames and, 2 frames in 6, shifts plane A by -$140 px)
 * In:  x, y = the spawn position (Obj_SpawnEnemy's d4/d5)
 * Out: script = Scr2_04A1A8; Obj_SpawnEnemy's results (a6 = the object, d0 = 0 / -1)
 * Evidence: lea Scr2_04A1A8,a2 / jmp Obj_SpawnEnemy; Scr2_04A1A8: Scene_EndAfterDelay 0/$78,
 *           Op2_StepVelocity, Op2_ApplyMove, Op2_CallNativeRel 6 -> $4A1C8; TAS f503
 */
void BatmobileIntro_SpawnAnimator(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                                  uint32_t *a3, uint32_t *a4, uint32_t a6)
{
    *script = Scr2_04A1A8;
    dk::jump(at_Obj_SpawnEnemy);
}
