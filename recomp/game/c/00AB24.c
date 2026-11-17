#include "game.h"

/* Fade_InFromPale6 - jumps the palette to a pale level (+$A, toward white) and fades it back to
 * normal at one step per 6 frames, without the white CRAM flash.
 * In:  none
 * Out: g_FadeDelay = g_FadeDelayCount = 5, g_FadeLevel = $A, g_FadeTarget = 0; ccr of the clr.b
 * Evidence: move.b #5,g_FadeDelayCount / move.b #5,g_FadeDelay / move.b #$A,g_FadeLevel / clr.b
 *           g_FadeTarget / rts; pointers at $0467CC, $046836, $0468AA, $046900 (script natives);
 *           3 corpus runs, TAS frame 130232 (the Mr. Freeze fight)
 */
void Fade_InFromPale6(dk::Flags *ccr)
{
    g_FadeDelayCount = 5;
    g_FadeDelay = 5;
    g_FadeLevel = 0x0A;
    g_FadeTarget = 0;
    *ccr = dk::Flags::test(0, 1, ccr->x);
}
