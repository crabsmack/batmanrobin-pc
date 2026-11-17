#include "game.h"

/* the coroutine's next step after the native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_025D0A = 0x025D0Au;

/* Boss_Stage2_WaitFloorStill - boss phase-2 coroutine 1 native: yields while the arena floor is
 * still turning, else goes on to wait for the next turn (Boss_Stage2_WaitFloorTurn at $025D0E)
 * In:  g_Stage2Vars2->roll_speed; the running object's registers (for the script)
 * Out: turning: Boss_Stage2_FloorWaitYield (the yield); still: the script continues at
 *      Scr2_025D0A (jsr $7FE.w)
 * Evidence: tst.l g_SceneVars2+$12 / bne loc_025D14 (Boss_Stage2_FloorWaitYield) / jsr $7FE.w;
 *           the coroutine starts at $025CF8 when phase 2 begins; TAS f33171 (one frame)
 */
void Boss_Stage2_WaitFloorStill(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                                uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    if (g_Stage2Vars2->roll_speed != 0) {
        dk::jump(at_Boss_Stage2_FloorWaitYield);   /* bne into the next file */
        return;
    }
    dk::jump_script(at_VM2_Enter, Scr2_025D0A);
}
