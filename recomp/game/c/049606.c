#include "game.h"

/* the skyline's parallax command list (ROM) */
static constexpr uint32_t Tbl_049616 = 0x049616u;

enum { CITY_SPEED = 2 };   /* camera pixels a frame */

/* GameOver_ScrollCity - per-frame native of the game-over director: moves the camera 2 px right
 * and runs the parallax command list Tbl_049616, so the skyline scrolls by.
 * In:  the director's registers (Scroll_RunParallax's)
 * Out: g_CameraX's integer part + 2; list = Tbl_049616, then Scroll_RunParallax's results
 * Evidence: addq.w #2,g_CameraX / lea Tbl_049616,a5 / jmp Scroll_RunParallax; Op2_CallNativeRel
 *           from the director Scr2_0494EA ($49504); scenario-game-over f42833-f43858
 */
void GameOver_ScrollCity(uint32_t *d0, uint32_t d1, uint32_t *d2, uint32_t *d3, uint16_t *d6, uint16_t *d7,
                         uint32_t *a1, uint32_t *list, uint32_t *a6)
{
    /* addq.w on the 16.16 long's high word: the fraction stays, no carry out of the word */
    uint32_t cam = (uint32_t)g_CameraX;
    g_CameraX = (((cam >> 16) + CITY_SPEED) << 16) | (cam & 0xFFFFu);
    *list = Tbl_049616;
    dk::jump(at_Scroll_RunParallax);
}
