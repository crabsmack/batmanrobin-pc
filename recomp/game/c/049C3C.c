#include "game.h"

/* the PLAYER / SELECT caption object's a2 script (ROM) */
static constexpr uint32_t Scr2_049C48 = 0x049C48u;

/* PlayerSelect_SpawnCaption - spawns the PLAYER / SELECT caption object (Scr2_049C48): it starts at
 * screen (25, $137) cruising up, eases to a stop after 30 frames, and draws both words every frame
 * (PlayerSelect_DrawCaption).
 * In:  x, y = the spawn position (Obj_SpawnEnemy's d4/d5)
 * Out: script = Scr2_049C48; Obj_SpawnEnemy's results (a6 = the object)
 * Evidence: lea Scr2_049C48,a2 / jmp Obj_SpawnEnemy; Scr2_049C48: Op2_PlaceOnScreen $19/$137,
 *           Op2_CruiseUp, coroutine $49C70, Op2_CallNativeRel $1E -> $49C88; TAS f431
 */
void PlayerSelect_SpawnCaption(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                               uint32_t *a3, uint32_t *a4, uint32_t a6)
{
    *script = Scr2_049C48;
    dk::jump(at_Obj_SpawnEnemy);
}
