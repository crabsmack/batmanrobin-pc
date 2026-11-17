#include "game.h"

/* Tint_StepLine3 - steps the line tint and applies it to palette line 3 (the same offset for red,
 * green and blue): the boss-hit flash of the sprites' palette line.
 * In:  the g_Tint* state
 * Out: level (d0.b) = g_TintLevel, g_Pal3TintR/G/B = it; step (d1) as Tint_Step; ccr of the
 *      last move.b of the level (Z when the flash is over)
 * Evidence: bsr Tint_Step / 3x move.b d0,g_Pal3Tint*; the native $01A182 sets the sprites'
 *           palette 3 while the result is nonzero; TAS 11,575 frames from 4540
 */
void Tint_StepLine3(uint8_t *level, uint32_t *step, dk::Flags *ccr)
{
    Tint_Step(level, step);
    g_Pal3TintR = *level;
    g_Pal3TintG = *level;
    g_Pal3TintB = *level;
    *ccr = dk::Flags::test(*level, 1, ccr->x);
}
