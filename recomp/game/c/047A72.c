#include "game.h"

enum {
    EASE_STEP = -0x800,               /* the speed eases toward the wanted one by -$800 a frame */
    SPEED_SLOWEST = (int32_t)0xFFFE0000, /* -2 px a frame */
    SPEED_FASTEST = (int32_t)0xFFF40000, /* -12 px a frame */
    RING = 0x400000,                   /* one ring of the tunnel: 64 px */
    LINES = 224,                       /* the buffer: a plane A, plane B word pair per line */
    CENTRE_LINE = 112,
    LINE_DIVISOR = 0xE0,
    CENTRE_FACTOR = 0x70,
    TERM_START = 0x1BE,                /* the stepping's second difference */
};

static uint32_t with_low_word(uint32_t r, uint16_t v) { return (r & 0xFFFF0000u) | v; }
static uint32_t swap(uint32_t v) { return (v << 16) | (v >> 16); }

/* Stage4_TubeScroll - the ring tunnel's scroll every frame: the speed eases toward the wanted one
 * (-$800 a frame, clamped -2..-12 px), the position moves by it and wraps within one 64-px ring;
 * then the line pairs (plane A, plane B words of mode $10) get x = position +/- an offset that
 * grows by incremental quadratic stepping from the screen's edges (plane A, lines 0 and 223
 * inward) and from the centre line 112 (plane B, outward), the two meeting.
 * In:  g_SceneVars (TubeScrollVars: want_speed, speed, pos)
 * Out: vars->speed, vars->pos; the line buffer g_HScrollLinesA..g_HIntTblFront ($FFE1D2-$FFE551);
 *      d1 = the centre offset, d2 = $0000:the stepping's accumulator, d3 = $FFFF:its first
 *      difference, d4.w = its second, d5 = the per-line step, d6 = the last centre value (halves
 *      swapped); a2 / a5 = where the edge lines stopped, a3 / a4 the centre lines
 * Evidence: cmp.l / blt / addi.l #$FFFFF800 / the $FFFE0000 / $FFF40000 clamps / the $400000 wrap /
 *           lea g_HScrollLinesA,a2 / g_HScrollLinesB,a3 / a4 / g_HIntTblFront,a5 / divs.w #$E0 /
 *           muls.w #$70 / the d2/d3/d4 step loop (bpl, ble); only caller Stage4_RunTube's frame
 *           loop; TAS 122681-128749
 */
void Stage4_TubeScroll(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint32_t *d5, uint32_t *d6,
                       uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5)
{
    TubeScrollVars *vars = (TubeScrollVars *)g_SceneVars;
    int32_t pos = vars->pos;
    int32_t want = vars->want_speed;
    int32_t speed = vars->speed;
    if (want < speed)
        speed += EASE_STEP;
    else
        speed = want;
    if (speed > SPEED_SLOWEST)
        speed = SPEED_SLOWEST;
    if (speed < SPEED_FASTEST)
        speed = SPEED_FASTEST;
    vars->speed = speed;
    pos = (int32_t)((uint32_t)pos + (uint32_t)speed);
    if (pos >= 0)
        pos -= RING;
    if (pos < -RING)
        pos += RING;
    vars->pos = pos;

    u16 *edge_top = (u16 *)g_HScrollLinesA;              /* line 0, forward */
    u16 *edge_bottom = (u16 *)g_HScrollLinesA + 2 * LINES; /* past line 223, backward */
    u16 *centre_down = (u16 *)g_HScrollLinesA + 2 * CENTRE_LINE;
    u16 *centre_up = centre_down;

    uint32_t edge = 0;                                     /* d0 */
    int16_t q = (int16_t)((int32_t)pos / LINE_DIVISOR);    /* divs.w: |pos| / $E0 always fits */
    uint32_t step = (uint32_t)(int32_t)q;                  /* d5 (ext.l) */
    uint32_t centre = (uint32_t)((int32_t)q * CENTRE_FACTOR);   /* d1 */
    uint32_t acc = 0;                                      /* d2 */
    uint32_t diff = 0xFFFFFFFEu;                           /* d3: moveq #-2 */
    uint16_t term = TERM_START;                            /* d4 */
    uint32_t value = *d6;

    for (;;) {
        edge += step;
        diff = with_low_word(diff, (uint16_t)(diff + 4));
        acc = with_low_word(acc, (uint16_t)(acc - diff));
        if ((int16_t)acc >= 0) {
            /* bpl: another pair of centre lines */
        } else {
            acc = with_low_word(acc, (uint16_t)(acc + term));
            term = (uint16_t)(term - 4);
            centre -= step;
            if ((int32_t)centre > (int32_t)edge)
                break;                                     /* the two have met */
            uint32_t out = swap((uint32_t)vars->pos + edge), in = swap((uint32_t)vars->pos - edge);
            *edge_top++ = (uint16_t)out;
            *edge_top++ = (uint16_t)in;
            *--edge_bottom = (uint16_t)in;
            *--edge_bottom = (uint16_t)out;
        }
        value = swap((uint32_t)vars->pos + centre);
        uint32_t in = swap((uint32_t)vars->pos - centre);
        *centre_down++ = (uint16_t)value;
        *centre_down++ = (uint16_t)in;
        *--centre_up = (uint16_t)in;
        *--centre_up = (uint16_t)value;
    }
    *d1 = centre;
    *d2 = acc;
    *d3 = diff;
    *d4 = term;
    *d5 = step;
    *d6 = value;
    *a2 = dk::addr(edge_top);
    *a3 = dk::addr(centre_down);
    *a4 = dk::addr(centre_up);
    *a5 = dk::addr(edge_bottom);
}
