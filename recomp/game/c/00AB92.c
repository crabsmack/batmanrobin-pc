#include "game.h"

/* Fade_MinTint - the smallest of the 12 per-line tint offsets (r/g/b of palette lines 0-3), at
 * most +$E.
 * In:  the line tint records g_PalLineTint[0-3] (the reload flags skipped)
 * Out: d0.b = the minimum (signed)
 * Evidence: the mirror of Fade_MaxTint: move.b #$E,d0, then cmp.b -(a0),d0 / blt / move.b
 *           (a0),d0 over $F4D5 down to $F4C7; only caller Fade_StartOutWhite
 */
uint8_t Fade_MinTint(void)
{
    int8_t min = 0x0E;
    for (int line = 3; line >= 0; line--) {
        const PalLineTint *t = &g_PalLineTint[line];
        if (t->b <= min) min = t->b;
        if (t->g <= min) min = t->g;
        if (t->r <= min) min = t->r;
    }
    return (uint8_t)min;
}
