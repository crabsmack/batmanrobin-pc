#include "game.h"

/* units of the unrolled run Span_ClearLinesRun ($00D2DC-$00D81B, 6 bytes each) */
static constexpr int CLEARLINES_UNITS = (0x00D81C - 0x00D2DC) / 6;   /* 224 */

/* Span_ClearLines - fills count lines of the span tables with 'no span': the H-scroll word, and a
 * V-scroll that counts down one per line, so every line keeps showing the same plane row (row 0
 * when vscroll = -line). The original jumps into the unrolled Span_ClearLinesRun count units
 * before its rts; the C loops over the same writes, in the same order.
 * In:  hscroll = d0.w; vscroll = the first line's V-scroll (d2.w); count = lines (d7.w, 0-224);
 *      dst_h/dst_v = the H-scroll and V-scroll line tables (a0/a1: g_HIntTblBack)
 * Out: dst_h/dst_v past the lines; vscroll - count; count = -6 x count (the jump's displacement)
 * Evidence: add.w d7,d7 / move.w d7,-(a7) / add.w d7,d7 / add.w (a7)+,d7 / neg.w d7 /
 *           jmp loc_00D81C(pc,d7.w); callers pass d0 = 0 and d2 = 0 or -(lines already done):
 *           Span_ClearScreen(Alt), the triangle and disc code, $40980; 12 corpus runs
 */
void Span_ClearLines(uint16_t hscroll, uint16_t *vscroll, uint16_t *count, uint32_t *dst_h, uint32_t *dst_v)
{
    *count = (uint16_t)-(uint16_t)(*count * 6);
    int16_t d = (int16_t)*count;
    if (d > 0 || -d % 6 != 0 || -d / 6 > CLEARLINES_UNITS)
        dk::fatal("Span_ClearLines: displacement $%04X lands outside the unrolled run", (unsigned)*count);
    PTR(u16) ph, pv;
    ph.v = *dst_h;
    pv.v = *dst_v;
    u16 *h = ph, *v = pv;
    for (int n = -d / 6; n > 0; n--) {
        *h++ = hscroll;
        *v++ = *vscroll;
        *vscroll = (uint16_t)(*vscroll - 1);
    }
    *dst_h = ADDR68(h);
    *dst_v = ADDR68(v);
}
