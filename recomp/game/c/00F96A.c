#include "game.h"

/* The span triangles ($00F96A-$00FE0D): Span_ClearScreen, Span_DrawTriangle, Span_ClearScreenAlt and
 * Span_DrawTriangleDither (the stage 2 flying section's beam).
 *
 * A triangle is drawn into the two span tables (per line an H-scroll word, the span's left edge,
 * and a V-scroll word, width - line) as two halves of 16.16 ramps (Span_FillRamps, or the dithered
 * Span_FillRampsDither0/1), with the lines above and below cleared (Span_ClearLines), counting the
 * 224 screen lines down in g_SpanLinesLeft.
 *
 * The two triangle routines keep everything in registers and their sorts swap whole registers
 * (exg.l), the callers' high words with them; every register comes out (the measured contract,
 * widened for the tail jump to Span_ClearLines), so the C keeps the 68000's registers as the
 * fields of TriRegs and does each instruction's arithmetic on them. */

/* the registers both triangle routines work in */
struct TriRegs {
    uint32_t d0, d1, d2, d3, d4, d5, d6, d7;
    uint32_t a0, a1, a2, a3;
};

static int16_t wd(uint32_t r) { return (int16_t)(uint16_t)r; }
static uint32_t setw(uint32_t r, int v) { return (r & 0xFFFF0000u) | (uint16_t)v; }
static uint32_t ext(uint32_t r) { return (uint32_t)(int32_t)(int16_t)(uint16_t)r; }
static uint32_t swapw(uint32_t r) { return r << 16 | r >> 16; }
static uint32_t asr1(uint32_t r) { return (uint32_t)((int32_t)r >> 1); }

/* divs.w: the 32-bit dividend by a signed word; the remainder in the high word, the quotient in
   the low. A quotient that doesn't fit a word sets V and leaves the dividend unchanged. */
static uint32_t divs_w(uint32_t dividend, int16_t divisor)
{
    if (divisor == 0)
        dk::fatal("Span_DrawTriangle: divs.w by zero");
    int64_t q = (int64_t)(int32_t)dividend / divisor;
    if (q < -32768 || q > 32767)
        return dividend;
    int32_t r = (int32_t)dividend % divisor;
    return ((uint32_t)(uint16_t)r << 16) | (uint16_t)q;
}

/* A 16.16 value times a signed word, as the asm does it: muls.w of the fraction (signed, so a
 * fraction >= $8000 borrows: its integer part + 1 first) plus muls.w of the integer part << 16.
 * a2 = the adjusted value, a3 = the fraction's product (left in them, as the asm leaves them). */
static uint32_t mul_fixed(TriRegs *r, uint32_t value, int16_t k)
{
    r->a2 = value;                                       /* movea.l d1,a2 */
    if ((int16_t)value < 0)
        r->a2 += 0x10000u;                               /* adda.l #$10000,a2 */
    r->a3 = (uint32_t)((int32_t)(int16_t)value * k);     /* muls.w d2,d1 / movea.l d1,a3 */
    uint32_t whole = (uint32_t)((int32_t)(int16_t)(r->a2 >> 16) * k);   /* swap / muls.w */
    return (whole << 16) + r->a3;                        /* swap / clr.w / add.l a3 */
}

/* cmp.w ya,yb / blt keep / bne swap / cmp.w xa,xb / bgt keep: the vertex with the smaller y (on a
 * tie the larger x) stays first */
static void sort_pair(uint32_t *ya, uint32_t *yb, uint32_t *xa, uint32_t *xb)
{
    if (wd(*ya) < wd(*yb) || (wd(*ya) == wd(*yb) && wd(*xa) > wd(*xb)))
        return;
    uint32_t t = *ya; *ya = *yb; *yb = t;
    t = *xa; *xa = *xb; *xb = t;
}

/* jsr Span_FillRamps: d7 lines of the ramps d0/d1 (H) and d2/d3 (V) */
static void fill_ramps(TriRegs *r)
{
    uint16_t lines = (uint16_t)r->d7;
    Span_FillRamps(&r->d0, r->d1, &r->d2, r->d3, &lines, &r->a0, &r->a1);
    r->d7 = setw(r->d7, lines);
}

/* movem.l d0/d2/d7 / moveq #0,d0 / moveq #0,d2 / jsr Span_ClearLines / movem: the d2 lines above
 * the screen-clipped triangle's top; only a0/a1 come back advanced */
static void clear_above(TriRegs *r)
{
    uint16_t vscroll = 0;
    uint16_t lines = (uint16_t)r->d2;
    Span_ClearLines(0, &vscroll, &lines, &r->a0, &r->a1);
}

/* lsr.w #1,d4 / bcc / jsr Span_FillRampsDither1 or Span_FillRampsDither0: d4's bit 0 (lines left +
 * g_FrameParity) picks the phase that keeps the checkerboard aligned with the lines already drawn */
static void fill_dithered(TriRegs *r)
{
    bool odd = (r->d4 & 1) != 0;
    r->d4 = setw(r->d4, (uint16_t)r->d4 >> 1);
    uint16_t clear = (uint16_t)r->d5;
    if (odd)
        Span_FillRampsDither1(&r->d0, &r->d1, &r->d2, r->d3, &r->d4, &clear, &r->d7, &r->a0, &r->a1);
    else
        Span_FillRampsDither0(&r->d0, &r->d1, &r->d2, r->d3, &r->d4, &clear, &r->d7, &r->a0, &r->a1);
    r->d5 = setw(r->d5, clear);
}

/* d4 = g_SpanLinesLeft with g_FrameParity added to its low byte (move.w / add.b) */
static uint32_t phase_of(uint32_t d4)
{
    d4 = setw(d4, (uint16_t)g_SpanLinesLeft);
    return (d4 & 0xFFFFFF00u) | (uint8_t)((uint8_t)d4 + (uint8_t)g_FrameParity);
}

/* cmp.w (g_SpanLinesLeft).w,dN / ble / move.w (g_SpanLinesLeft).w,dN: a part's lines capped at
 * the lines left; true when the cap went negative (bmi: nothing left, the routine returns) */
static bool cap_lines(uint32_t *lines)
{
    if (wd(*lines) <= (int16_t)(uint16_t)g_SpanLinesLeft)
        return false;
    *lines = setw(*lines, (uint16_t)g_SpanLinesLeft);
    return wd(*lines) < 0;
}

enum TriExit { TRI_RETURN, TRI_CLEAR_BELOW };

/* The two triangle routines, instruction for instruction ($00F976 and $00FBB0): `dither` selects
 * the half-width ramps and the dithered fills. Ends with the rts or with the jump that clears the
 * lines below (TRI_CLEAR_BELOW: d0/d2/d7 set for Span_ClearLines). */
static TriExit draw_triangle(TriRegs *r, bool dither)
{
    g_SpanLinesLeft = 0xE0;
    r->d2 = setw(r->d2, (uint16_t)g_TriY0);
    r->d7 = setw(r->d7, (uint16_t)g_TriY1);
    r->d6 = setw(r->d6, (uint16_t)g_TriY2);
    r->d0 = setw(r->d0, (uint16_t)g_TriX0);
    r->d1 = setw(r->d1, (uint16_t)g_TriX1);
    r->d4 = setw(r->d4, (uint16_t)g_TriX2);
    sort_pair(&r->d2, &r->d7, &r->d0, &r->d1);
    sort_pair(&r->d7, &r->d6, &r->d1, &r->d4);
    sort_pair(&r->d2, &r->d7, &r->d0, &r->d1);   /* d2/d0 top, d7/d1 middle, d6/d4 bottom */

    int height = wd(r->d6) - wd(r->d2);           /* sub.w d2,d6 / ble: a flat triangle */
    r->d6 = setw(r->d6, height);
    if (height <= 0)
        goto clear_below;
    r->d3 = setw(r->d3, wd(r->d4));
    r->d5 = setw(r->d5, wd(r->d4) - wd(r->d1));   /* bottom x - middle x */
    r->d1 = setw(r->d1, wd(r->d1) - wd(r->d0));   /* middle x - top x */
    r->d3 = setw(r->d3, wd(r->d3) - wd(r->d0));   /* bottom x - top x */
    r->d0 = swapw(r->d0) & 0xFFFF0000u;           /* the top x as 16.16 */

    {   /* the top edge's slope (d1), over the upper half's height d7 */
        int upper = wd(r->d7) - wd(r->d2);
        r->d7 = setw(r->d7, upper);
        if (upper <= 0) {
            r->d1 = swapw(r->d1) & 0xFFFF0000u;   /* no upper half: the x step as 16.16 */
        } else if (wd(r->d7) > 8) {
            r->d1 = ext(divs_w(ext(r->d1) << 8, wd(r->d7))) << 8;
        } else {
            /* short halves: 3 more fraction bits before the divide */
            r->d1 = ext(divs_w(ext(r->d1) << 5, wd(r->d7))) << 3 << 8;
        }
    }
    r->d3 = ext(divs_w(ext(r->d3) << 8, wd(r->d6))) << 8;   /* the long edge's slope */
    {   /* the bottom edge's slope (d5), over the lower half's height d6 */
        int lower = wd(r->d6) - wd(r->d7);
        r->d6 = setw(r->d6, lower);
        if (lower <= 0)
            r->d5 = swapw(r->d5) & 0xFFFF0000u;
        else
            r->d5 = ext(divs_w(ext(r->d5) << 8, wd(r->d6))) << 8;
    }
    /* cmp.l d1,d3 / ble: d1/d4 become the left edges' slopes, d3/d5 the right ones */
    if (!((int32_t)r->d3 <= (int32_t)r->d1)) {
        r->d4 = r->d5;
        r->d5 = r->d3;
        if (!(wd(r->d7) > 0))
            r->d0 += r->d1;   /* no upper half: the lower half starts at the middle vertex */
    } else {
        uint32_t t = r->d1; r->d1 = r->d3; r->d3 = t;
        r->d4 = r->d1;
    }
    r->d3 -= r->d1;           /* the width ramps: right slope - left slope */
    r->d5 -= r->d4;
    if (dither) {             /* half-width spans */
        r->d3 = asr1(r->d3);
        r->d5 = asr1(r->d5);
    }

    r->d2 = ext(r->d2);       /* ext.l d2 / bpl: the top on screen? */
    if ((int32_t)r->d2 < 0) {
        r->d7 = setw(r->d7, wd(r->d7) + wd(r->d2));
        if (wd(r->d7) >= 0) {
            /* the top is above the screen, the middle on it: advance both ramps by -top lines */
            uint32_t d1 = r->d1, d3 = r->d3;
            r->d2 = setw(r->d2, -wd(r->d2));
            r->d1 = mul_fixed(r, r->d1, wd(r->d2));
            r->d3 = mul_fixed(r, r->d3, wd(r->d2));
            r->d2 = r->d3;
            r->d0 += r->d1;
            r->d1 = d1;
            r->d3 = d3;
            r->d2 += 0x10000u;
            if (wd(r->d7) > 0)
                goto upper_half;
            goto lower_half;
        }
        /* the middle is above the screen too: start the lower half part-way down */
        {
            int left = wd(r->d6) + wd(r->d7);
            r->d6 = setw(r->d6, left);
            if (left <= 0)
                goto clear_below;
        }
        uint32_t d4 = r->d4, d5 = r->d5;
        r->d2 = setw(r->d2, wd(r->d2) - wd(r->d7));
        r->d7 = setw(r->d7, -wd(r->d7));
        r->d2 = setw(r->d2, -wd(r->d2));
        r->d1 = mul_fixed(r, r->d1, wd(r->d2));
        r->d4 = mul_fixed(r, r->d4, wd(r->d7));
        r->d5 = mul_fixed(r, r->d5, wd(r->d7));
        if (wd(r->d2) != 0)
            r->d3 = mul_fixed(r, r->d3, wd(r->d2));
        r->d3 += r->d5;
        r->d2 = r->d3;
        r->d1 += r->d4;
        r->d0 += r->d1;
        r->d4 = d4;
        r->d5 = d5;
        r->d2 += 0x10000u;
        goto lower_half;
    }

    /* the top on screen: clear the lines above it */
    if (wd(r->d2) >= 0xE0)
        goto clear_below;
    g_SpanLinesLeft = (uint16_t)(g_SpanLinesLeft - (uint16_t)r->d2);
    clear_above(r);
    r->d2 = swapw(1u - r->d2);    /* neg.l / addq.l #1 / swap: V = 1 - top, as 16.16 */
    if (wd(r->d7) > 0)
        goto upper_half;
    r->d2 += r->d3;
    goto lower_half;

upper_half:
    if (dither) {
        uint32_t d4 = r->d4, d5 = r->d5, d6 = r->d6;   /* movem.l d4-d6 */
        r->d4 = phase_of(r->d4);
        r->d3 -= 0x10000u;
        cap_lines(&r->d7);
        g_SpanLinesLeft = (uint16_t)(g_SpanLinesLeft - (uint16_t)r->d7);
        fill_dithered(r);
        r->d4 = d4;
        r->d5 = d5;
        r->d6 = d6;
    } else {
        r->d3 -= 0x10000u;
        cap_lines(&r->d7);
        g_SpanLinesLeft = (uint16_t)(g_SpanLinesLeft - (uint16_t)r->d7);
        fill_ramps(r);
    }

lower_half:
    if (dither) {
        r->d7 = setw(r->d7, wd(r->d6));
        r->d1 = r->d4;
        r->d3 = r->d5;
        r->d4 = phase_of(r->d4);
        r->d3 -= 0x10000u;
        if (wd(r->d7) <= 0)
            goto clear_below;
        if (cap_lines(&r->d7))
            return TRI_RETURN;
        g_SpanLinesLeft = (uint16_t)(g_SpanLinesLeft - (uint16_t)r->d7);
        fill_dithered(r);
    } else {
        r->d5 -= 0x10000u;
        if (wd(r->d6) <= 0)
            goto clear_below;
        if (cap_lines(&r->d6))
            return TRI_RETURN;
        g_SpanLinesLeft = (uint16_t)(g_SpanLinesLeft - (uint16_t)r->d6);
        r->d7 = setw(r->d7, wd(r->d6));
        r->d1 = r->d4;
        r->d3 = r->d5;
        fill_ramps(r);
    }

clear_below:
    r->d7 = setw(r->d7, (uint16_t)g_SpanLinesLeft);
    if (wd(r->d7) <= 0)
        return TRI_RETURN;
    r->d2 = setw(r->d2, 0xFF20 + wd(r->d7));   /* V = -(224 - left): line n's no-span value */
    r->d0 = 0;
    return TRI_CLEAR_BELOW;
}

/* Span_ClearScreen - clears the whole span picture: all 224 lines get H-scroll 0 and V-scroll
 * -line (Span_ClearLines, jumped to).
 * In:  a0/a1 = the H-scroll and V-scroll line tables
 * Out: as Span_ClearLines; going in hscroll = 0 (d0), vscroll = 0 (d2), lines = $E0 (d7)
 * Evidence: move.w #$E0,d7 / moveq #0,d0 / moveq #0,d2 / jmp Span_ClearLines; jmp from $334D2
 *           when no shape is drawn; d0 widened to an output: Span_ClearLines reads it
 */
void Span_ClearScreen(uint32_t *hscroll, uint32_t *vscroll, uint16_t *lines, uint32_t a0, uint32_t a1)
{
    (void)a0; (void)a1;
    *lines = 0xE0;
    *hscroll = 0;
    *vscroll = 0;
    dk::jump(at_Span_ClearLines);
}

/* Span_DrawTriangle - draws a filled triangle into the span tables: sorts the vertices g_TriX0/Y0
 * .. X2/Y2 by y, clears the lines above, fills both halves with Span_FillRamps (H = the left edge,
 * V = width - line) and clears the lines below, clipping at the top and at 224 lines.
 * In:  a0 = the H-scroll line table (g_HScrollLinesA), a1 = the V-scroll one (g_HIntTblBack);
 *      g_TriX0..g_TriY2; the other registers' high words ride along the sorts
 * Out: g_SpanLinesLeft counted down; every register as the asm leaves it; with lines left, the
 *      jump to Span_ClearLines with d0 = 0, d2 = $FF20 + lines left, d7 = lines left
 * Evidence: the three compare/exg sorts, the divs slopes into 16.16, the muls clipping, the jsrs
 *           of Span_ClearLines / Span_FillRamps and the closing jmp Span_ClearLines; d0/d7/a0/a1
 *           widened to outputs: that jmp reads them; called by $471C0
 */
void Span_DrawTriangle(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                       uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3)
{
    TriRegs r = { *d0, *d1, *d2, *d3, *d4, *d5, *d6, *d7, *a0, *a1, *a2, *a3 };
    TriExit exit = draw_triangle(&r, false);
    *d0 = r.d0; *d1 = r.d1; *d2 = r.d2; *d3 = r.d3; *d4 = r.d4; *d5 = r.d5; *d6 = r.d6; *d7 = r.d7;
    *a0 = r.a0; *a1 = r.a1; *a2 = r.a2; *a3 = r.a3;
    if (exit == TRI_CLEAR_BELOW)
        dk::jump(at_Span_ClearLines);
}

/* Span_ClearScreenAlt - the second copy of Span_ClearScreen (224 lines of no span), placed before
 * Span_DrawTriangleDither.
 * In:  a0/a1 = the H-scroll and V-scroll line tables
 * Out: as Span_ClearLines; going in hscroll = 0 (d0), vscroll = 0 (d2), lines = $E0 (d7)
 * Evidence: byte for byte Span_ClearScreen's 3 instructions; jmp from $471C0 when its beam slot is
 *           empty; d0/d7 widened to outputs: Span_ClearLines reads them
 */
void Span_ClearScreenAlt(uint32_t *hscroll, uint32_t *vscroll, uint32_t *lines, uint32_t a0, uint32_t a1)
{
    (void)a0; (void)a1;
    *lines = (*lines & 0xFFFF0000u) | 0xE0;
    *hscroll = 0;
    *vscroll = 0;
    dk::jump(at_Span_ClearLines);
}

/* Span_DrawTriangleDither - Span_DrawTriangle with half-width ramps and the dithered fills
 * (Span_FillRampsDither0/1 by the lines left and the frame parity): the translucent beam triangle
 * of the stage 2 flying-section boss.
 * In:  as Span_DrawTriangle
 * Out: as Span_DrawTriangle (d3/d6/a2/a3 changed, no caller reads them)
 * Evidence: the same sort and clipping, then asr.l #1 of both width ramps, movem.l d4-d6 around
 *           the upper half's fill, and the phase (g_SpanLinesLeft + g_FrameParity) lsr.w #1;
 *           d0/d2/a0/a1 widened to outputs: the closing jmp Span_ClearLines reads them; jsr from
 *           $334D2
 */
void Span_DrawTriangleDither(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d4, uint32_t *d5, uint32_t d6,
                             uint32_t *d7, uint32_t *a0, uint32_t *a1)
{
    TriRegs r = { *d0, *d1, *d2, 0, *d4, *d5, d6, *d7, *a0, *a1, 0, 0 };
    TriExit exit = draw_triangle(&r, true);
    *d0 = r.d0; *d1 = r.d1; *d2 = r.d2; *d4 = r.d4; *d5 = r.d5; *d7 = r.d7;
    *a0 = r.a0; *a1 = r.a1;
    if (exit == TRI_CLEAR_BELOW)
        dk::jump(at_Span_ClearLines);
}
