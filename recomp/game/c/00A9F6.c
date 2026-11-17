#include "game.h"

enum { FADE_FULL = 0x0E };   /* g_FadeLevel +$E = all white, -$E = all black */

/* Fade_StartIn - starts a scene's fade in: the palette starts fully faded (white when the level
 * is already at +$E, a scene entered after a fade to white; black otherwise) and steps to normal
 * brightness, one step per 3 frames.
 * In:  g_FadeLevel
 * Out: g_FadeTarget = 0, g_FadeDelayCount = 0, g_FadeDelay = 2, g_FadeLevel = g_FadeLevelVInt =
 *      +$E or -$E; level (d0.b) = the same (an output, not a clobber: Scene_InitStage and
 *      Scene_InitPlain end in a bra here, so their measured d0 is this one)
 * Evidence: clr.b g_FadeTarget / clr.b g_FadeDelayCount / move.b #2,g_FadeDelay / move.b #$E,d0 /
 *           cmp.b g_FadeLevel,d0 / ble / neg.b d0; the tail (bra) of Scene_InitStage and
 *           Scene_InitPlain; TAS 25 frames, one per scene start
 */
void Fade_StartIn(uint8_t *level_out)
{
    g_FadeTarget = 0;
    g_FadeDelayCount = 0;
    g_FadeDelay = 2;
    int8_t level = FADE_FULL;
    if (level > (int8_t)g_FadeLevel)              /* ble: keep +$E only when the level is >= it */
        level = -level;
    g_FadeLevelVInt = (uint8_t)level;
    g_FadeLevel = (uint8_t)level;
    *level_out = (uint8_t)level;
}
