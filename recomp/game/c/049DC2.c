#include "game.h"

/* the SEGA logo object's a2 script (ROM) */
static constexpr uint32_t Scr2_049DCE = 0x049DCEu;

/* SegaLogo_SpawnCycler - spawns the SEGA logo object (Scr2_049DCE): it ends the scene after 120
 * frames and rotates the logo's colours (SegaLogo_RotatePalette) every 3 frames.
 * In:  x, y = the spawn position (Obj_SpawnEnemy's d4/d5)
 * Out: script = Scr2_049DCE; Obj_SpawnEnemy's results (a6 = the object)
 * Evidence: lea Scr2_049DCE,a2 / jmp Obj_SpawnEnemy; Scr2_049DCE: Scene_EndAfterDelay 0/$78,
 *           Op2_SetTimer3 $30, Op2_CallNativeRel 6 -> $49DEC; TAS f18
 */
void SegaLogo_SpawnCycler(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                          uint32_t *a3, uint32_t *a4, uint32_t a6)
{
    *script = Scr2_049DCE;
    dk::jump(at_Obj_SpawnEnemy);
}
