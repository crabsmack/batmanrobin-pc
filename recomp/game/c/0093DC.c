#include "game.h"

enum {
    VIEW_SPR_ORIGIN = 0x20,       /* g_SprOffsetX/Y = this - the view (Spr_BuildSAT adds it to every sprite) */
    VIEW_ONSCREEN_MARGIN = 0x18,  /* the on-screen tests' origin: $18 px left of / above the camera */
};

static uint32_t low_word(uint32_t r, uint16_t w)
{
    return (r & 0xFFFF0000u) | w;
}

/* Camera_UpdateView - per frame: steps the screen shake and derives the view values from the
 * camera (the sprite offsets from the shaken view, the on-screen test origin, the movement area),
 * and counts two shared timers down to 0.
 * In:  g_CameraX/Y; g_MoveAreaOfsX/Y
 * Out: Shake_Update's (g_ViewX/Y, d1/d2); g_SprOffsetX/Y = $20 - the view's integer x/y;
 *      g_OnScreenLeft/Top = camera - $18; g_MoveAreaLeft/Top = camera + g_MoveAreaOfsX/Y;
 *      g_ScreenClearTimer, g_ScriptTimer - 1 unless 0; d1 low word = g_SprOffsetX, d0 = g_MoveAreaTop
 * Evidence: bsr.w Shake_Update / move.w #$20,d1 / sub.w g_ViewY,d1 / move.w d1,g_SprOffsetY / ... /
 *           move.l g_CameraY,d0 / add.l g_MoveAreaOfsY,d0 / move.l d0,g_MoveAreaTop / tst.w / subq.w
 *           (both timers); the tail of Camera_TrackPlayersX and called by the other stage loops
 *           ($15916, $1FD76, $3FE94, $400E4...)
 */
void Camera_UpdateView(uint32_t *d0, uint32_t *d1, uint32_t *d2)
{
    Shake_Update(d1, d2);
    *d1 = low_word(*d1, (uint16_t)(VIEW_SPR_ORIGIN - (uint16_t)(g_ViewY >> 16)));
    g_SprOffsetY = (uint16_t)*d1;
    *d1 = low_word(*d1, (uint16_t)(VIEW_SPR_ORIGIN - (uint16_t)(g_ViewX >> 16)));
    g_SprOffsetX = (uint16_t)*d1;
    g_OnScreenLeft = (uint16_t)((uint16_t)(g_CameraX >> 16) - VIEW_ONSCREEN_MARGIN);
    g_OnScreenTop = (uint16_t)((uint16_t)(g_CameraY >> 16) - VIEW_ONSCREEN_MARGIN);
    g_MoveAreaLeft = (uint32_t)g_CameraX + (uint32_t)g_MoveAreaOfsX;
    *d0 = (uint32_t)g_CameraY + (uint32_t)g_MoveAreaOfsY;
    g_MoveAreaTop = *d0;
    if (g_ScreenClearTimer != 0)
        g_ScreenClearTimer--;
    if (g_ScriptTimer != 0)
        g_ScriptTimer--;
}
