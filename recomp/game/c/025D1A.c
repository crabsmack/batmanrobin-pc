#include "game.h"

/* the boss coroutine after the native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_025D1E = 0x025D1Eu;

/* Boss_Stage2_FloorTurning - the turning exit of Boss_Stage2_WaitFloorTurn: back into the boss
 * coroutine at the descent (Op2_SetCruiseY $855, Op2_SetAccelYForDist, then
 * Boss_Stage2_EndPlayerTilts)
 * In:  the running object's registers (for the script)
 * Out: the script continues at Scr2_025D1E (jsr $7FE.w)
 * Evidence: jsr $7FE.w alone; bne target in Boss_Stage2_WaitFloorTurn; TAS f33526 (one frame);
 *           png f033600: the hull lower over a tilted floor
 */
void Boss_Stage2_FloorTurning(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                              uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    dk::jump_script(at_VM2_Enter, Scr2_025D1E);
}
