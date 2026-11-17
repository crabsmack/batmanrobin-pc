#include "game.h"

/* the narrator's a2 script (ROM) */
static constexpr uint32_t Scr2_04A728 = 0x04A728u;

/* Ending_SpawnCellNarrator - spawns the Freeze-cell ending's narrator (Scr2_04A728, a boss-class
 * object): its coroutine types five text pages with fades, palette loads and the snow globe between
 * them, then ends the scene; its loop runs Ending_CellTextStep every frame
 * In:  x, y = the spawn position (Obj_SpawnBoss's d4/d5)
 * Out: script (a2) = Scr2_04A728; Obj_SpawnBoss's results
 * Evidence: lea Scr2_04A728,a2 / jmp Obj_SpawnBoss; Scr2_04A728: SetSpeed $100,
 *           StartCoroutine0Rel -> $4A74E (TextMap_RunInline $4A7FE / Fade_StartOutBlack /
 *           StartA6Script $4A5A0 / ... / Scene_EndAfterDelay 0); called by Ending_FreezeCell; f15109
 */
void Ending_SpawnCellNarrator(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                              uint32_t *a3, uint32_t *a4, uint32_t a6)
{
    *script = Scr2_04A728;
    dk::jump(at_Obj_SpawnBoss);
}
