#include "game.h"

/* Span_ClearLinesRun - the unrolled body of Span_ClearLines entered at its first unit: all 224
 * lines of 'no span': the H-scroll word to dst_h and the V-scroll word to dst_v, the V-scroll one
 * less each line (so every line shows the same plane row).
 * In:  hscroll = d0.w; vscroll = the first line's V-scroll (d2.w); dst_h/dst_v = the H-scroll and
 *      V-scroll line tables (a0/a1)
 * Out: dst_h/dst_v past the lines; vscroll - 224
 * Evidence: 224 x (move.w d0,(a0)+ / move.w d2,(a1)+ / subq.w #1,d2) then the rts at $00D81C;
 *           entered by the computed jump at $00D828 (Span_ClearLines; dkit.toml computed_entry_runs)
 */
void Span_ClearLinesRun(uint16_t hscroll, uint16_t *vscroll, uint32_t *dst_h, uint32_t *dst_v)
{
    uint16_t lines = 224;
    Span_ClearLines(hscroll, vscroll, &lines, dst_h, dst_v);
}
