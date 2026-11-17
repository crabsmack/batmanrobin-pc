#include "game.h"

/* The span tables: per screen line an H-scroll word (the span's left edge) and a V-scroll word
 * (Span_DrawTriangle: width - line, so the line shows the plane row as wide as the span). A line
 * with no span has V-scroll -line (Span_ClearLines: V + line = 0). The disc adds its half-width to
 * the V word of an empty line, or widens a span already there to cover both. */

static constexpr uint16_t DISC_LAST_LINE = 0xDF;   /* lines 0-223 */
static constexpr uint16_t DISC_FIRST_CLIP = 0xE0;  /* a centre - radius at or past line 224 */
static constexpr uint16_t DITHER_CLEAR = 0xFFFE;   /* d5: and.w clears the H word's bit 0 */
static constexpr uint16_t DITHER_SET = 0x0001;     /* d0's low word (top half) / d7 (bottom half) */

/* What one line's merge leaves in d3/d4 (Span_DrawDiscDither's outputs come from the last line). */
struct DiscRegs {
    uint16_t edge;   /* d3 */
    uint16_t vsum;   /* d4 */
};

/* One line of the disc: half = the half-width at this line, the left edge x - half with bit 0
 * forced (and.w clear or or.w set), merged into the line's H word *h and V word *v (both advanced).
 * An empty line ((a1) + line = 0) takes the disc's span; otherwise the left edge moves left when
 * the disc's is further left (and the V word, the width, grows by what the edge moved or by the
 * room left, halved), or the V word grows to reach the disc's right side. */
static void merge_line(u16 *&h, u16 *&v, uint16_t centre_x, uint16_t half, bool set_bit, uint16_t line,
                       DiscRegs *r)
{
    uint16_t edge = (uint16_t)(centre_x - half);
    edge = set_bit ? (uint16_t)(edge | DITHER_SET) : (uint16_t)(edge & DITHER_CLEAR);
    uint16_t vsum = (uint16_t)(*v + line);
    if (vsum == 0) {
        *h++ = edge;                                   /* move.w d3,(a0)+ */
        *v = (uint16_t)(*v + half);                    /* add.w d2,(a1)+ */
        v++;
        r->edge = edge;
        r->vsum = vsum;
        return;
    }
    uint16_t moved = (uint16_t)(edge - *h);            /* sub.w (a0),d3 */
    if ((int16_t)moved < 0) {                          /* the disc's left edge is further left */
        *h = (uint16_t)(*h + moved);
        h++;
        vsum = (uint16_t)(vsum - half);
        moved = (uint16_t)((int16_t)moved >> 1);       /* asr.w #1 */
        if ((int16_t)(uint16_t)(vsum - moved) < 0)     /* cmp.w d3,d4 / bmi */
            *v = (uint16_t)(*v - vsum);
        else
            *v = (uint16_t)(*v - moved);
        v++;
    } else {
        h++;
        moved = (uint16_t)((int16_t)moved >> 1);
        uint16_t grow = (uint16_t)(half + moved - vsum);   /* add.w d3,d2 / sub.w d4,d2 / bpl */
        if ((int16_t)grow >= 0)
            *v = (uint16_t)(*v + grow);
        v++;
    }
    r->edge = moved;
    r->vsum = vsum;
}

/* the dither phase of the first line: bit 0 of (line + g_FrameParity) as bytes (add.b / lsr.b) */
static bool starts_set(uint16_t line)
{
    return (((uint8_t)line + (uint8_t)g_FrameParity) & 1) != 0;
}

/* Span_DrawDiscDither - adds a dithered filled disc to the span tables, merging each line with a
 * span already there (the union of the two), from the half-width table g_CircleSpans: the top
 * half reads it forward (rows 0..r), the bottom half backward; clipped to lines 0-223.
 * In:  radius = d0.w (its sign ignored); centre_x/centre_y = d1.w/d2.w (screen); h_lines/v_lines =
 *      the H-scroll and V-scroll line tables (a0/a1); g_CircleSpans built for this radius
 * Out: the disc's lines merged; edge/vsum = the last line's d3/d4 (as merge_line leaves them),
 *      clear = $FFFE (d5) once a line is drawn, line = the line index (d6: 0 at the first clip
 *      test), rows = the row counter (d7.w: $FFFF after the top half's dbra; 1 in the bottom
 *      half; 2 x the visible rows when the top half is clipped away), top/bottom = the table
 *      pointers (a2/a3) where the reads stopped
 * Evidence: tst.w d0 / bpl / neg.w d0; lea g_CircleSpans,a2 / lea 2(a2,d0.w),a3 / adda.w d0,a3;
 *           the clip arithmetic against $E0 / $DF; the two copies of each half's line loop
 *           (and.w d5,d3 / or.w d0,d3; or.w d7,d3 in the bottom half) chosen by
 *           (d6 + g_FrameParity) lsr.b; jmp from $334D2 and Span_ClearDrawDisc; TAS frames
 *           77312-79108
 */
void Span_DrawDiscDither(uint16_t radius, uint16_t centre_x, uint16_t centre_y, uint16_t *edge, uint16_t *vsum,
                         uint16_t *clear, uint32_t *line, uint16_t *rows, uint32_t h_lines, uint32_t v_lines,
                         uint32_t *top, uint32_t *bottom)
{
    uint16_t r = radius;
    if ((int16_t)r < 0)
        r = (uint16_t)-r;
    uint32_t a2 = ADDR68(g_CircleSpans);
    uint32_t a3 = a2 + 2 + (uint32_t)(int32_t)(int16_t)r;   /* lea 2(a2,d0.w),a3 */
    a3 += (uint32_t)(int32_t)(int16_t)r;                     /* adda.w d0,a3: past row r */
    uint16_t n = 0;                                          /* d6 (moveq: high word 0) */
    *line = 0;
    PTR(u16) ph, pv;
    ph.v = h_lines;
    pv.v = v_lines;
    u16 *h = ph, *v = pv;
    DiscRegs regs = { *edge, *vsum };
    uint16_t d7 = r;
    uint16_t count = r;                                      /* d0.w: the bottom half's rows - 1 */
    bool top_half = true;

    uint16_t first = (uint16_t)(centre_y - r);               /* d3: the top row's line */
    regs.edge = first;
    if ((int16_t)first < 0) {
        int sum = (int16_t)d7 + (int16_t)first;              /* add.w d3,d7 / bgt */
        d7 = (uint16_t)sum;
        if (sum > 0) {
            regs.edge = (uint16_t)(first << 1);
            a2 -= (uint32_t)(int32_t)(int16_t)regs.edge;     /* suba.w: skip the clipped rows */
        } else {
            int left = (int16_t)count + (int16_t)d7;         /* add.w d7,d0 / ble */
            count = (uint16_t)left;
            if (left <= 0)
                goto done;
            d7 = (uint16_t)(d7 << 1);
            a3 += (uint32_t)(int32_t)(int16_t)d7;            /* adda.w d7,a3 */
            top_half = false;
        }
    } else {
        if ((int16_t)first >= (int16_t)DISC_FIRST_CLIP)
            goto done;
        n = (uint16_t)(n + first);
        regs.edge = (uint16_t)(first + first);
        h += (int16_t)regs.edge >> 1;                        /* adda.w d3,a0 / adda.w d3,a1 */
        v += (int16_t)regs.edge >> 1;
    }

    if (top_half) {
        /* loc_00FE4E: clip the top half's rows at line 223 */
        regs.edge = (uint16_t)(DISC_LAST_LINE - n - d7);
        if ((int16_t)regs.edge < 0) {
            d7 = (uint16_t)(d7 + regs.edge);
            if ((int16_t)d7 < 0)
                goto done;
        }
        *clear = DITHER_CLEAR;
        PTR(const u16) src;
        src.v = a2;
        const u16 *s = src;
        bool set_bit = starts_set(n);
        for (;;) {
            merge_line(h, v, centre_x, *s++, set_bit, n, &regs);
            if (d7-- == 0)                                   /* dbra d7 */
                break;
            n++;
            set_bit = !set_bit;
        }
        a2 = ADDR68(s);
        n++;                                                 /* loc_00FF06; swap d0: count = r */
    }

    /* loc_00FF0A: the bottom half, clipped at line 223 */
    regs.edge = (uint16_t)(DISC_LAST_LINE - n - count);
    if ((int16_t)regs.edge < 0) {
        count = (uint16_t)(count + regs.edge);
        if ((int16_t)count < 0)
            goto done;
    }
    d7 = DITHER_SET;
    *clear = DITHER_CLEAR;
    {
        PTR(const u16) src;
        src.v = a3;
        const u16 *s = src;
        bool set_bit = starts_set(n);
        for (;;) {
            merge_line(h, v, centre_x, *--s, set_bit, n, &regs);
            if (count-- == 0)                                /* dbra d0 */
                break;
            n++;
            set_bit = !set_bit;
        }
        a3 = ADDR68(s);
    }

done:
    *edge = regs.edge;
    *vsum = regs.vsum;
    *line = n;
    *rows = d7;
    *top = a2;
    *bottom = a3;
}
