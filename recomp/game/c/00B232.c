#include "game.h"

/* The ramp fills ($00B232-$00C907): Mem_FillRampRun + Mem_FillRamp, Span_FillRampsRun +
 * Span_FillRamps, and Mem_FillWordsRun (the run Mem_FillWords, the next file, jumps into).
 *
 * The originals are unrolled runs of 224 units (one per screen line) entered at a computed offset:
 * `lsl.w #3,d7 / neg.w d7 / jmp end(pc,d7.w)` lands count units before the run's closing rts.
 * Mem_FillRampRun's unit (8 bytes) is `swap d0 / move.w d0,(a0)+ / swap d0 / add.l d1,d0`: the
 * integer word of a 16.16 accumulator, then the step; Span_FillRampsRun's (16 bytes) does the same
 * for a second ramp d2 += d3 into (a1)+. The C loops over the same writes, in the same order. A
 * count past a run's start would run into the code before it: a fatal here (never in the
 * corpus). */

static constexpr int RAMP_UNITS = (0x00B932 - 0x00B232) / 8;      /* 224 */
static constexpr int RAMPS_UNITS = (0x00C73C - 0x00B93C) / 16;    /* 224 */
static constexpr int WORDS_UNITS = (0x00C906 - 0x00C746) / 2;     /* 224 */

/* the jump's displacement (count x unit bytes, negated) as units back from the run's rts */
static int units_of(uint16_t displacement, int unit_bytes, int units, const char *run)
{
    int16_t d = (int16_t)displacement;
    if (d > 0 || (-d) % unit_bytes != 0 || -d / unit_bytes > units)
        dk::fatal("%s: displacement $%04X lands outside the unrolled run", run, (unsigned)displacement);
    return -d / unit_bytes;
}

static u16 *words_at(uint32_t a)
{
    PTR(u16) p;
    p.v = a;
    return p;
}

/* `n` units of one ramp: the accumulator's integer word to *dst, then the step */
static void ramp(uint32_t *acc, uint32_t step, uint32_t *dst, int n)
{
    u16 *w = words_at(*dst);
    for (int i = 0; i < n; i++) {
        *w++ = (uint16_t)(*acc >> 16);
        *acc += step;
    }
    *dst = ADDR68(w);
}

/* `n` units of two ramps, interleaved as the original's units are */
static void ramps(uint32_t *acc_a, uint32_t step_a, uint32_t *acc_b, uint32_t step_b, uint32_t *dst_a,
                  uint32_t *dst_b, int n)
{
    u16 *a = words_at(*dst_a);
    u16 *b = words_at(*dst_b);
    for (int i = 0; i < n; i++) {
        *a++ = (uint16_t)(*acc_a >> 16);
        *acc_a += step_a;
        *b++ = (uint16_t)(*acc_b >> 16);
        *acc_b += step_b;
    }
    *dst_a = ADDR68(a);
    *dst_b = ADDR68(b);
}

/* Mem_FillRampRun - the unrolled body of Mem_FillRamp entered at its first unit: 224 words of
 * the ramp.
 * In:  acc = the 16.16 start (d0); step (d1); dst (a0)
 * Out: dst past the words, acc = start + 224 x step
 * Evidence: 224 x (swap d0 / move.w d0,(a0)+ / swap d0 / add.l d1,d0), then the rts at $00B932;
 *           entered by the computed jump at $00B938 (dkit.toml computed_entry_runs)
 */
void Mem_FillRampRun(uint32_t *acc, uint32_t step, uint32_t *dst)
{
    ramp(acc, step, dst, RAMP_UNITS);
}

/* Mem_FillRamp - writes `count` words of a linear ramp: the integer parts of acc, acc + step,
 * acc + 2 x step ... (16.16), e.g. per-line scroll that grows with the line.
 * In:  acc = the start (d0); step (d1); count (d7.w: 0-224); dst (a0); d2/d3/a1 pass through
 *      (the measured contract keeps them: the computed jump hides the run from the analysis)
 * Out: dst past the words, acc = start + count x step (d7 = -8 x count: clobbered)
 * Evidence: lsl.w #3,d7 / neg.w d7 / jmp loc_00B932(pc,d7.w) into Mem_FillRampRun; callers
 *           $15D90 (5 bands of g_HScrollLinesB) and $24660 (stage 2's line tables)
 */
void Mem_FillRamp(uint32_t *acc, uint32_t step, uint32_t *d2, uint32_t d3, uint16_t count, uint32_t *dst,
                  uint32_t *a1)
{
    (void)d2; (void)d3; (void)a1;
    uint16_t displacement = (uint16_t)-(uint16_t)(count << 3);
    ramp(acc, step, dst, units_of(displacement, 8, RAMP_UNITS, "Mem_FillRamp"));
}

/* Span_FillRampsRun - the unrolled body of Span_FillRamps entered at its first unit: 224 lines of
 * both ramps.
 * In:  h/h_step (d0/d1) into h_dst (a0); v/v_step (d2/d3) into v_dst (a1); 16.16
 * Out: h_dst/v_dst past the lines, h/v advanced 224 steps
 * Evidence: 224 x (swap d0 / move.w d0,(a0)+ / swap d0 / add.l d1,d0 / swap d2 /
 *           move.w d2,(a1)+ / swap d2 / add.l d3,d2), the rts at $00C73C; entered by the computed
 *           jump at $00C742 (dkit.toml computed_entry_runs)
 */
void Span_FillRampsRun(uint32_t *h, uint32_t h_step, uint32_t *v, uint32_t v_step, uint32_t *h_dst, uint32_t *v_dst)
{
    ramps(h, h_step, v, v_step, h_dst, v_dst, RAMPS_UNITS);
}

/* Span_FillRamps - fills `count` lines of a line-scroll span: H-scroll words from the ramp h into
 * h_dst and V-scroll words from the ramp v into v_dst (16.16 accumulators).
 * In:  h/h_step (d0/d1); v/v_step (d2/d3); count (d7.w: 0-224); h_dst (a0, a line table);
 *      v_dst (a1, the H-int table g_HIntTblBack)
 * Out: h_dst/v_dst past the lines, h/v advanced; count = -16 x count (the jump's displacement)
 * Evidence: lsl.w #4,d7 / neg.w d7 / jmp loc_00C73C(pc,d7.w) into Span_FillRampsRun; callers
 *           Span_DrawTriangle (the triangle's two halves) and $24900 (stage 2)
 */
void Span_FillRamps(uint32_t *h, uint32_t h_step, uint32_t *v, uint32_t v_step, uint16_t *count, uint32_t *h_dst,
                    uint32_t *v_dst)
{
    *count = (uint16_t)-(uint16_t)(*count << 4);
    ramps(h, h_step, v, v_step, h_dst, v_dst, units_of(*count, 16, RAMPS_UNITS, "Span_FillRamps"));
}

/* Mem_FillWordsRun - the unrolled body of Mem_FillWords entered at its first unit: 224 words of
 * the value.
 * In:  value (d0.w); dst (a0)
 * Out: dst past the words
 * Evidence: 224 x move.w d0,(a0)+ then the rts at $00C906; entered only by the computed jump at
 *           $00C90C (Mem_FillWords, whose C loops itself; dkit.toml computed_entry_runs)
 */
void Mem_FillWordsRun(uint16_t value, uint32_t *dst)
{
    u16 *w = words_at(*dst);
    for (int i = 0; i < WORDS_UNITS; i++)
        *w++ = value;
    *dst = ADDR68(w);
}
