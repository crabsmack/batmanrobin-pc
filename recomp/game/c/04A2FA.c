#include "game.h"

/* the object's a2 script (ROM) */
static constexpr uint32_t Scr2_04A306 = 0x04A306u;

/* StageCard_SpawnDirector2 - spawns stage 2's card director (Scr2_04A306): scene end after $C8 frames,
 * palette line 3, then the LEVEL TWO title and the A TWO SIDED STORY subtitle 30 frames apart
 * In:  x, y = the spawn position (Obj_SpawnEnemy's d4/d5)
 * Out: script = Scr2_04A306; Obj_SpawnEnemy's results (a6 = the object, d0 = 0 / -1)
 * Evidence: lea Scr2_04A306,a2 / jmp Obj_SpawnEnemy; Scr2_04A306 = the stage 1 director's layout
 *           (Scene_EndAfterDelay $C8, Op2_LoadPalette3, two CallNativeRelSaved +$10/+$3A); called with
 *           jsr from StageCard_Stage2
 */
void StageCard_SpawnDirector2(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                              uint32_t *a3, uint32_t *a4, uint32_t a6)
{
    *script = Scr2_04A306;
    dk::jump(at_Obj_SpawnEnemy);
}
