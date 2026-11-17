#include "game.h"

/* Fade_MaxTint - the largest of the 12 per-line tint offsets (r/g/b of palette lines 0-3), at
 * least -$E.
 * In:  the line tint records g_PalLineTint[0-3] (the reload flags skipped)
 * Out: d0.b = the maximum (signed)
 * Evidence: move.b #$F2,d0 / lea g_PalLineCache,a0 / moveq #3,d1; per line subq.l #1,a0 then 3x
 *           cmp.b -(a0),d0 / bgt / move.b (a0),d0: the bytes $F4D5 down to $F4C7, every 4th
 *           skipped; only caller Fade_StartOutBlack
 */
uint8_t Fade_MaxTint(void)
{
    int8_t max = -0x0E;
    for (int line = 3; line >= 0; line--) {       /* the asm walks down: b, g, r of line 3 first */
        const PalLineTint *t = &g_PalLineTint[line];
        if (t->b >= max) max = t->b;
        if (t->g >= max) max = t->g;
        if (t->r >= max) max = t->r;
    }
    return (uint8_t)max;
}
