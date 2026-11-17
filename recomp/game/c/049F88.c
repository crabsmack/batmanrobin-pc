#include "game.h"

/* the object's a2 script (ROM) */
static constexpr uint32_t Scr2_049F94 = 0x049F94u;

/* Copyright_SpawnText - spawns the copyright notice object (script Scr2_049F94, boss class): ends the
 * scene after 180 frames, draws the notice text once and kills itself
 * In:  x, y = the spawn position (Obj_SpawnBoss's d4/d5)
 * Out: script = Scr2_049F94; Obj_SpawnBoss's results (a6 = the object, d0 = 0 / -1)
 * Evidence: lea Scr2_049F94,a2 / jmp Obj_SpawnBoss; Scr2_049F94 is an a2 script: Op2_CallNative
 *           Scene_EndAfterDelay 0/$B4, Op2_Native $49FA0, then Op2_Kill $95C at $49FB6; TAS f164
 */
void Copyright_SpawnText(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                         uint32_t *a3, uint32_t *a4, uint32_t a6)
{
    *script = Scr2_049F94;
    dk::jump(at_Obj_SpawnBoss);
}
