#include "game.h"

/* the continue controller's a2 script (ROM) */
static constexpr uint32_t Scr2_0491A0 = 0x0491A0u;

/* Continue_SpawnController - spawns the continue screen's controller object (Scr2_0491A0): it
 * hovers on an oscillator and branches on the credits (Continue_BranchHasCredits) into the
 * countdown and the Start check.
 * In:  x, y = the spawn position (Obj_SpawnEnemy's d4/d5)
 * Out: script = Scr2_0491A0; Obj_SpawnEnemy's results (a6 = the object, d0 = 0 / -1)
 * Evidence: lea Scr2_0491A0,a2 / jmp Obj_SpawnEnemy; Scr2_0491A0: SetPos 0 0, AllocOsc / SetOscY,
 *           Op2_Native into Continue_BranchHasCredits; only caller Continue_Run; scenario-continue
 *           f41999
 */
void Continue_SpawnController(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                              uint32_t *a3, uint32_t *a4, uint32_t a6)
{
    *script = Scr2_0491A0;
    dk::jump(at_Obj_SpawnEnemy);
}
