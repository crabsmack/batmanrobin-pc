#include "game.h"

/* the gate camera's a2 script (ROM) */
static constexpr uint32_t Scr2_04853A = 0x04853Au;

/* Story_SpawnGateCamera - spawns the gate scene's camera object (Scr2_04853A): ends the scene after
 * $12C frames, rises $120 px in a $D20-tick arc and draws two sprites; its native Story_GateScroll
 * turns its y into the planes' scroll.
 * In:  x, y = the spawn position (Obj_SpawnEnemy's d4/d5)
 * Out: script = Scr2_04853A; Obj_SpawnEnemy's results (a6 = the object, d0 = 0 / -1)
 * Evidence: lea Scr2_04853A,a2 / jmp Obj_SpawnEnemy; Scr2_04853A: CallNative Scene_EndAfterDelay
 *           $12C, JumpAnim $FEE0 $D20, CallNativeRel Story_GateScroll; callers Story_AsylumGate and
 *           Ending_AsylumGate; idle 1722 + 3925n
 */
void Story_SpawnGateCamera(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                           uint32_t *a3, uint32_t *a4, uint32_t a6)
{
    *script = Scr2_04853A;
    dk::jump(at_Obj_SpawnEnemy);
}
