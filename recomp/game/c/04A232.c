#include "game.h"

/* the object's a2 script (ROM) */
static constexpr uint32_t Scr2_04A23E = 0x04A23Eu;

/* StageCard_SpawnDirector1 - spawns stage 1's card director (Scr2_04A23E): ends the scene after $C8
 * frames, loads palette line 3, then spawns the title after 30 frames and the subtitle 30 frames later
 * In:  x, y = the spawn position (Obj_SpawnEnemy's d4/d5)
 * Out: script = Scr2_04A23E; Obj_SpawnEnemy's results (a6 = the object, d0 = 0 / -1)
 * Evidence: lea Scr2_04A23E,a2 / jmp Obj_SpawnEnemy; Scr2_04A23E: CallNative Scene_EndAfterDelay $C8,
 *           Op2_LoadPalette3, coroutine SetTimer0 $1E0 (30 frames at speed $100) / CallNativeRelSaved
 *           -> StageCard_SpawnTitle1 / StageCard_SpawnSubtitle1; f773 (title f802, subtitle f832)
 */
void StageCard_SpawnDirector1(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                              uint32_t *a3, uint32_t *a4, uint32_t a6)
{
    *script = Scr2_04A23E;
    dk::jump(at_Obj_SpawnEnemy);
}
