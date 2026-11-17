#include "game.h"

enum {
    CORRIDOR_LINES = 0xE0,           /* the 224-line budget */
    CORRIDOR_SECTION = 0x80,         /* each perspective section: $80 units deeper */
    CORRIDOR_WALL_STEP = 0x20,       /* the middle wall's depth per section */
    CORRIDOR_PERSPECTIVE = 0x5000,   /* the span ramps' scale numerator (divu by the span height) */
};

/* muls.w / mulu.w: the low words into a long */
static uint32_t Cor_Muls(uint32_t a, uint32_t b)
{
    return (uint32_t)((int32_t)(int16_t)a * (int32_t)(int16_t)b);
}
static uint32_t Cor_Mulu(uint32_t a, uint32_t b)
{
    return (uint32_t)(uint16_t)a * (uint16_t)b;
}

/* divu.w / divs.w: remainder:quotient; a quotient that doesn't fit a word leaves the dividend */
static uint32_t Cor_Divu(uint32_t n, uint16_t d)
{
    uint32_t q = n / d;
    return q > 0xFFFFu ? n : ((n % d) << 16) | q;
}
static uint32_t Cor_Divs(uint32_t n, int16_t d)
{
    int64_t q = (int64_t)(int32_t)n / d;
    if (q < -32768 || q > 32767)
        return n;
    return ((uint32_t)(uint16_t)((int32_t)n % d) << 16) | (uint16_t)q;
}

static uint32_t Cor_Swap(uint32_t v) { return (v << 16) | (v >> 16); }
static uint32_t Cor_Low(uint32_t r, uint32_t w) { return (r & 0xFFFF0000u) | (uint16_t)w; }
static int16_t Cor_W(uint32_t r) { return (int16_t)(uint16_t)r; }
static uint32_t Cor_Ext(uint32_t r) { return (uint32_t)(int32_t)Cor_W(r); }   /* ext.l */

/* sub.w s,(lines_left): true while lines are left after it (bgt) */
static bool Cor_TakeLines(FlightCorridorVars *v, uint16_t n)
{
    int16_t before = v->lines_left;
    v->lines_left = (int16_t)(uint16_t)(before - n);
    return (int32_t)before - (int16_t)n > 0;   /* sub then bgt: the true difference */
}

/* Flight_DrawCorridor - draws the flying section's corridor into plane B's line H-scroll and the
 * H-int line V-scroll table, section by section until the 224 lines are done: a perspective
 * ceiling span (Span_FillRamps), the middle wall's bands (Flight_CopyBandLines), a floor span,
 * then a plain fill (Mem_FillWords2), each section $80 deeper
 * In:  g_FlightCorridor vpos / phase / band[] (Flight_UpdateCorridor); g_HIntTblBack
 * Out: the line tables filled; work values +8 (phase / 4) and +$C ((phase - phase / 4) >> 8);
 *      the last fill is a jump (Span_FillRamps / Mem_FillWords2 with the budget's remainder) or
 *      Flight_CopyBandLines returns past this; the registers as that last fill leaves them
 * Evidence: TAS f35433-f47954 every frame (11967 frames), level-skip f4424-f5883; line budget
 *           g_SceneVars+$10 = $E0; `sub.w d7,(g_SceneVars+$10) / bgt / add.w ... ,d7 / jmp` before
 *           each fill. The prototype makes d0/a0/a1 outputs (measured clobbers) so the jumps hand
 *           them on to Span_FillRamps / Mem_FillWords2
 */
void Flight_DrawCorridor(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint16_t *d6,
                         uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a4)
{
    FlightCorridorVars *v = g_FlightCorridor;
    *a0 = dk::addr(g_HScrollLinesB);
    *a1 = (uint32_t)(int32_t)(int16_t)(uint16_t)g_HIntTblBack;   /* movea.w */
    v->lines_left = CORRIDOR_LINES;
    *d4 = (uint32_t)v->phase;
    *d5 = (uint32_t)((int32_t)*d4 >> 2);
    v->work_quarter = (int32_t)*d5;
    *d4 -= *d5;
    *d4 = (uint32_t)((int32_t)*d4 >> 8);
    v->work_rest = (int32_t)*d4;
    *d4 = Cor_Low(*d4, (uint32_t)v->vpos >> 16);                  /* move.w: the integer word */
    *d5 = Cor_Low(*d5, (uint16_t)((Cor_W(*d4) >> 2) + 0x54 - 0x40));
    *d4 = Cor_Low(*d4, Cor_W(*d4) - 0x100);

    for (;;) {
        /* the ceiling span above this section's middle wall */
        *d6 = (uint16_t)(Cor_W(*d5) - Cor_W(*d4));
        if (Cor_W(*d5) > Cor_W(*d4)) {
            *d7 = Cor_Low(*d7, *d6);
            *d2 = Cor_Low(*d2, Cor_W(*d4) + CORRIDOR_SECTION);
            *d2 = Cor_Low(*d2, Cor_W(*d2) - Cor_W(*d5));
            if (Cor_W(*d2) < 0)                                   /* bpl: N of the result */
                *d7 = Cor_Low(*d7, Cor_W(*d7) + Cor_W(*d2));
            *d2 = Cor_Low(*d2, 0u - (uint16_t)*d4);
            if (Cor_W(*d2) < 0)
                *d2 = 0;
            bool lines = Cor_W(*d7) > Cor_W(*d2);                 /* sub.w / ble */
            *d7 = Cor_Low(*d7, Cor_W(*d7) - Cor_W(*d2));
            if (lines) {
                *d0 = Cor_Low(*d0, *d2);
                *d3 = Cor_Divu(CORRIDOR_PERSPECTIVE, *d6);
                *d2 = Cor_Mulu(*d3, *d2);
                *d3 = Cor_Low(*d3, Cor_W(*d3) - 0x100);
                *d3 = Cor_Ext(*d3) << 8;
                *d2 = Cor_Swap(*d2 << 8);
                *d2 = Cor_Low(*d2, Cor_W(*d2) - 0x60 + v->lines_left);
                *d1 = Cor_Divs((uint32_t)v->work_rest, (int16_t)*d6);
                *d1 = Cor_Low(*d1, 0u - (uint16_t)*d1);
                *d0 = Cor_Muls(*d1, *d0);
                *d1 = Cor_Ext(*d1) << 8;
                *d0 = (*d0 << 8) + (uint32_t)v->phase;
                *d2 = Cor_Swap(*d2) - 0x8000;
                *d0 -= 0x8000;
                uint16_t n = (uint16_t)*d7;
                if (!Cor_TakeLines(v, n)) {
                    *d7 = Cor_Low(*d7, n + v->lines_left);
                    dk::jump(at_Span_FillRamps);   /* the budget's last lines */
                    return;
                }
                Span_FillRamps(d0, *d1, d2, *d3, &n, a0, a1);
                *d7 = Cor_Low(*d7, n);
                *d2 = Cor_Swap(*d2);
            }
        }

        /* the middle wall: the bands this section shows */
        *d3 = Cor_Low(*d3, *d4);
        *d2 = Cor_Low(*d2, *d5);
        *d4 = Cor_Low(*d4, Cor_W(*d4) + CORRIDOR_SECTION);
        *d5 = Cor_Low(*d5, Cor_W(*d5) + CORRIDOR_WALL_STEP);
        *d7 = Cor_Low(*d7, *d2);
        bool above = Cor_W(*d2) <= Cor_W(*d3);                   /* sub.w d3,d2 / ble */
        *d2 = Cor_Low(*d2, Cor_W(*d2) - Cor_W(*d3));
        if (!above)
            *d2 = 0;
        *d7 = Cor_Low(*d7, Cor_W(*d7) - Cor_W(*d2));
        if (Cor_W(*d7) < 0)                                       /* bpl */
            *d2 = Cor_Low(*d2, Cor_W(*d2) + Cor_W(*d7));
        bool past = Cor_W(*d4) > Cor_W(*d5);                      /* sub.w d5,d7 / ble */
        *d7 = Cor_Low(*d7, Cor_W(*d4) - Cor_W(*d5));
        if (past)
            *d7 = 0;
        *d7 = Cor_Low(*d7, Cor_W(*d7) + Cor_W(*d2));
        int32_t wall = (int32_t)Cor_W(*d7) + CORRIDOR_WALL_STEP;  /* addi.w / ble: the true sum */
        *d7 = Cor_Low(*d7, (uint32_t)wall);
        if (wall > 0) {
            *a4 = dk::addr(&v->band[0]);
            *d3 = Cor_Low(*d3, (uint16_t)*d2 << 2);
            *a4 -= (uint32_t)(int32_t)Cor_W(*d3);                 /* suba.w */
            *d2 = Cor_Low(*d2, 0u - (uint16_t)*d2);
            *d2 = Cor_Low(*d2, Cor_W(*d2) - 0x10 + v->lines_left);
            *d0 = Cor_Low(*d0, (uint32_t)v->work_quarter >> 16);
            PTR(u16) h, l;
            PTR(const u16) b;
            h.v = *a0;
            l.v = *a1;
            b.v = *a4;
            u16 *hp = h, *lp = l;
            const u16 *bp = b;
            uint16_t n = (uint16_t)*d7;
            dk::Flags ccr = dk::Flags::test(0, 4);
            Flight_CopyBandLines((uint16_t)*d2, &n, &hp, &lp, &bp, &ccr);   /* may return past us */
            *d7 = Cor_Low(*d7, n);
            *a0 = dk::addr(hp);
            *a1 = dk::addr(lp);
            *a4 = dk::addr(bp);
        }

        /* the floor span below the middle wall */
        *d6 = (uint16_t)(Cor_W(*d4) - Cor_W(*d5));
        if (Cor_W(*d4) > Cor_W(*d5)) {
            *d7 = Cor_Low(*d7, *d6);
            *d2 = Cor_Low(*d2, Cor_W(*d4) - CORRIDOR_SECTION);
            *d2 = Cor_Low(*d2, Cor_W(*d2) - Cor_W(*d5));
            if (Cor_W(*d2) < 0)
                *d2 = 0;
            bool lines = Cor_W(*d7) > Cor_W(*d2);
            *d7 = Cor_Low(*d7, Cor_W(*d7) - Cor_W(*d2));
            if (lines) {
                *d0 = Cor_Low(*d0, *d2);
                *d3 = Cor_Divu(CORRIDOR_PERSPECTIVE, *d6);
                *d3 = Cor_Low(*d3, 0u - (uint16_t)*d3);
                *d2 = Cor_Muls(*d3, *d2);
                *d3 = Cor_Low(*d3, Cor_W(*d3) - 0x100);
                *d3 = Cor_Ext(*d3) << 8;
                *d2 = Cor_Swap(*d2 << 8);
                *d2 = Cor_Low(*d2, Cor_W(*d2) - 0x11 + v->lines_left);
                *d1 = Cor_Divs((uint32_t)v->work_rest, (int16_t)*d6);
                *d0 = Cor_Muls(*d1, *d0);
                *d1 = Cor_Ext(*d1) << 8;
                *d0 = (*d0 << 8) + (uint32_t)v->work_quarter;
                *d2 = Cor_Swap(*d2) + 0x8000;
                *d0 += 0x8000;
                uint16_t n = (uint16_t)*d7;
                if (!Cor_TakeLines(v, n)) {
                    *d7 = Cor_Low(*d7, n + v->lines_left);
                    dk::jump(at_Span_FillRamps);
                    return;
                }
                Span_FillRamps(d0, *d1, d2, *d3, &n, a0, a1);
                *d7 = Cor_Low(*d7, n);
                *d2 = Cor_Swap(*d2);
            }
        }

        /* the plain fill down to the next section */
        *d3 = Cor_Low(*d3, *d4);
        *d4 = Cor_Low(*d4, Cor_W(*d4) + CORRIDOR_SECTION);
        *d5 = Cor_Low(*d5, Cor_W(*d5) + CORRIDOR_WALL_STEP);
        bool deeper = Cor_W(*d4) > Cor_W(*d3);                   /* sub.w d3,d7 / ble */
        *d7 = Cor_Low(*d7, Cor_W(*d4) - Cor_W(*d3));
        if (!deeper)
            continue;
        *d2 = 0;
        if (Cor_W(*d3) < 0) {
            *d7 = Cor_Low(*d7, Cor_W(*d7) + Cor_W(*d3));
            *d2 = Cor_Low(*d2, Cor_W(*d2) - Cor_W(*d3));
        }
        *d2 = Cor_Low(*d2, Cor_W(*d2) - 0xE0 + v->lines_left);
        *d0 = Cor_Low(*d0, (uint32_t)v->phase >> 16);
        uint16_t n = (uint16_t)*d7;
        if (!Cor_TakeLines(v, n)) {
            *d7 = Cor_Low(*d7, n + v->lines_left);
            dk::jump(at_Mem_FillWords2);
            return;
        }
        uint16_t b = (uint16_t)*d2;
        Mem_FillWords2((uint16_t)*d0, &b, &n, a0, a1);
        *d2 = Cor_Low(*d2, b);
        *d7 = Cor_Low(*d7, n);
    }
}
