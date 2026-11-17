#include "game.h"

/* the object's a2 script (ROM) */
static constexpr uint32_t Scr2_04A3CC = 0x04A3CCu;

/* StageCard_SpawnDirector3 - spawns stage 3's card director (Scr2_04A3CC): scene end after $C8 frames,
 * then the LEVEL THREE title and the TEA TIME! subtitle 30 frames apart (no palette-3 load, unlike stages
 * 1, 2, 4)
 * In:  x, y = the spawn position (Obj_SpawnEnemy's d4/d5)
 * Out: script = Scr2_04A3CC; Obj_SpawnEnemy's results (a6 = the object, d0 = 0 / -1)
 * Evidence: lea Scr2_04A3CC,a2 / jmp Obj_SpawnEnemy; Scr2_04A3CC: Scene_EndAfterDelay $C8, two
 *           CallNativeRelSaved +$10/+$3A, no $0B76 op; called from StageCard_Stage3
 */
void StageCard_SpawnDirector3(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                              uint32_t *a3, uint32_t *a4, uint32_t a6)
{
    *script = Scr2_04A3CC;
    dk::jump(at_Obj_SpawnEnemy);
}
