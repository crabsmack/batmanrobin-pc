#include "game.h"

/* the text object's script after the jsr $7FE.w: Op2_ApplyMove, Text_DrawAtObject, yield */
static constexpr uint32_t Scr2_049664 = 0x049664u;

enum { CITY_SPEED_16_16 = 0x20000 };   /* GameOver_ScrollCity's 2 px a frame, 16.16 */

/* GameOver_TextKeepPace - native step of the GAME OVER text: adds 2 px to the frame's x move, so
 * the text keeps pace with the camera GameOver_ScrollCity moves 2 px a frame.
 * In:  d1 = the x move (16.16) from Op2_StepVelocity; the object's registers
 * Out: d1 += $20000, read by the next op (Op2_ApplyMove); the script continues at Scr2_049664
 * Evidence: addi.l #$20000,d1 / jsr $7FE.w between Op2_StepVelocity and Op2_ApplyMove in
 *           Scr2_04963C; scenario-game-over f42833-f43858
 */
void GameOver_TextKeepPace(uint32_t d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                           uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    *d1 += CITY_SPEED_16_16;
    dk::jump_script(at_VM2_Enter, Scr2_049664);
}
