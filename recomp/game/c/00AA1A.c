#include "game.h"

/* Fade_ToNormal - fades the palette back to normal brightness from wherever it is, one step per
 * 3 frames (an a2 script native).
 * In:  none
 * Out: g_FadeDelayCount = 0, g_FadeDelay = 2, g_FadeTarget = 0; ccr of the clr.b (Z set)
 * Evidence: clr.b g_FadeDelayCount / move.b #2,g_FadeDelay / clr.b g_FadeTarget / rts; the
 *           pointer at $04A778 (a scene script's native call); 2 corpus runs
 */
void Fade_ToNormal(dk::Flags *ccr)
{
    g_FadeDelayCount = 0;
    g_FadeDelay = 2;
    g_FadeTarget = 0;
    *ccr = dk::Flags::test(0, 1, ccr->x);
}
