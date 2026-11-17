#include "game.h"

/* Op2_Return in the boss coroutine (the bytes after Boss_Stage2_FloorWaitYield's jsr $7FE.w) */
static constexpr uint32_t Scr2_025D18 = 0x025D18u;

/* Boss_Stage2_FloorWaitYield - the yield of the boss's two floor waits: back into the coroutine
 * at its Op2_Return, so the wait re-runs next frame
 * In:  the running object's registers (for the script)
 * Out: the script continues at Scr2_025D18 (jsr $7FE.w)
 * Evidence: jsr $7FE.w; bne target of Boss_Stage2_WaitFloorStill, the fall-through of
 *           Boss_Stage2_WaitFloorTurn; Scr2_025D18 = Op2_Return; TAS f33171-f33526
 */
void Boss_Stage2_FloorWaitYield(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                                uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    dk::jump_script(at_VM2_Enter, Scr2_025D18);
}

/* Boss_Stage2_WaitFloorTurn - boss phase-2 coroutine 1 native, re-run each frame: yields until
 * the arena floor starts turning, then the descent (Boss_Stage2_FloorTurning)
 * In:  g_Stage2Vars2->roll_speed; the running object's registers (for the script)
 * Out: turning: Boss_Stage2_FloorTurning; still: the yield (Boss_Stage2_FloorWaitYield)
 * Evidence: tst.l g_SceneVars2+$12 / bne Boss_Stage2_FloorTurning, falling into the yield;
 *           TAS f33171-f33526 (356 frames, until the tilt at f33526)
 */
void Boss_Stage2_WaitFloorTurn(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                               uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    if (g_Stage2Vars2->roll_speed != 0) {
        dk::jump(at_Boss_Stage2_FloorTurning);   /* bne into the next file */
        return;
    }
    /* falls into the yield entry of this file: the same jsr $7FE.w */
    Boss_Stage2_FloorWaitYield(d0, d1, d2, d3, d4, d5, d6, d7, a1, a3, a4, a5, a6);
}
