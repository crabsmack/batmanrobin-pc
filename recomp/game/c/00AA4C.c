#include "game.h"

/* Fade_StartOutWhite - starts the fade to white, one step per 3 frames. The target is +$E minus
 * the smallest line tint offset, so even the darkest-tinted channel reaches white.
 * In:  the line tint records (Fade_MinTint)
 * Out: g_FadeTarget, g_FadeDelay = 2, g_FadeDelayCount = 0; registers preserved (movem)
 * Evidence: movem.l d0-d1/a0,-(a7) / clr.b g_FadeDelayCount / move.b #2,g_FadeDelay / bsr
 *           Fade_MinTint / neg.b d0 / addi.b #$E,d0 / move.b d0,g_FadeTarget / movem.l; only
 *           caller SceneCb_EndTimerWhite; 4 corpus runs
 */
void Fade_StartOutWhite(void)
{
    g_FadeDelayCount = 0;
    g_FadeDelay = 2;
    g_FadeTarget = (uint8_t)(0x0E - Fade_MinTint());
}
