#include "game.h"

/* Fade_StartOutBlack - starts the fade to black, one step per 3 frames. The target is -$E minus
 * the largest line tint offset, so even the brightest-tinted channel reaches black.
 * In:  the line tint records (Fade_MaxTint)
 * Out: g_FadeTarget, g_FadeDelay = 2, g_FadeDelayCount = 0; registers preserved (movem); ccr of
 *      the move.b of the target
 * Evidence: movem.l d0-d1/a0,-(a7) / clr.b g_FadeDelayCount / move.b #2,g_FadeDelay / bsr
 *           Fade_MaxTint / neg.b d0 / addi.b #$F2,d0 / move.b d0,g_FadeTarget / movem.l; callers
 *           Scene_FadeOutExit and the pointer at $04A760 (a script native); TAS 24 frames
 */
void Fade_StartOutBlack(dk::Flags *ccr)
{
    g_FadeDelayCount = 0;
    g_FadeDelay = 2;
    uint8_t target = (uint8_t)(0xF2 - Fade_MaxTint());
    g_FadeTarget = target;
    *ccr = dk::Flags::test(target, 1, ccr->x);
}
