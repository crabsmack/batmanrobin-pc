#include "game.h"

/* Mem_FillWords and Mem_FillLongs ($00C908-$00C9F9) and, between them, Mem_FillLongsRun.
 *
 * The originals are unrolled runs entered at a computed offset: `add.w d7,d7 / neg.w d7 /
 * jmp end(pc,d7.w)` lands count units before the run's closing rts. Mem_FillWords' run
 * (Mem_FillWordsRun, $00C746-$00C905, 224 `move.w d0,(a0)+`) is the end of the previous function
 * file; Mem_FillLongsRun ($00C910-$00C9EF, 112 `move.l d0,(a0)+`) is in this one. The C loops over
 * the same writes, in the same order. A count past a run's start would run into the code before
 * it: a fatal here (never in the corpus). */

static constexpr int FILLWORDS_UNITS = (0x00C906 - 0x00C746) / 2;   /* 224 */
static constexpr int FILLLONGS_UNITS = (0x00C9F0 - 0x00C910) / 2;   /* 112 */

/* The jump's displacement (d7 after add.w d7,d7 / neg.w d7) as units of 2 bytes back from the rts. */
static int units_of(uint16_t displacement, int units, const char *run)
{
    int16_t d = (int16_t)displacement;
    if (d > 0 || -d / 2 > units)
        dk::fatal("%s: displacement $%04X lands outside the unrolled run", run, (unsigned)displacement);
    return -d / 2;
}

/* `n` words of `value` from the 68000 address *dst on */
static void fill_words(uint16_t value, uint32_t *dst, int n)
{
    PTR(u16) p;
    p.v = *dst;
    u16 *w = p;
    for (int i = 0; i < n; i++)
        *w++ = value;
    *dst = ADDR68(w);
}

/* `n` longs of `value` from the 68000 address *dst on */
static void fill_longs(uint32_t value, uint32_t *dst, int n)
{
    PTR(u32) p;
    p.v = *dst;
    u32 *l = p;
    for (int i = 0; i < n; i++)
        *l++ = value;
    *dst = ADDR68(l);
}

/* Mem_FillWords - stores the word value count times at dst (up to 224: one per screen line).
 * In:  value = d0.w; count = d7.w (0-224); dst = the destination (a0); d1/d2/d3/a1 pass through
 *      (the measured contract keeps them: the computed jump hides the run from the analysis)
 * Out: dst past the words; count = -2 x count (the jump's displacement)
 * Evidence: add.w d7,d7 / neg.w d7 / jmp loc_00C906(pc,d7.w) into Mem_FillWordsRun; callers
 *           Scroll_FillBands (each parallax band's lines), $3BB88, $48656; 63 corpus runs
 */
void Mem_FillWords(uint32_t *value, uint32_t d1, uint32_t *d2, uint32_t d3, uint16_t *count, uint32_t *dst,
                   uint32_t *a1)
{
    (void)d1; (void)d2; (void)d3; (void)a1;
    *count = (uint16_t)-(uint16_t)(*count * 2);
    fill_words((uint16_t)*value, dst, units_of(*count, FILLWORDS_UNITS, "Mem_FillWords"));
}

/* Mem_FillLongsRun - the unrolled body of Mem_FillLongs entered at its first unit: 112 longs of
 * value (224 words: a whole screen's line table).
 * In:  value = d0.l; dst = the destination (a0)
 * Out: dst past the longs
 * Evidence: 112 x move.l d0,(a0)+ then the rts at $00C9F0; entered by the computed jump at $00C9F6
 *           (dkit.toml computed_entry_runs), so its start is that jump's largest count
 */
void Mem_FillLongsRun(uint32_t value, uint32_t *dst)
{
    fill_longs(value, dst, FILLLONGS_UNITS);
}

/* Mem_FillLongs - stores the long value count times at dst (up to 112 longs = 224 words).
 * In:  value = d0.l; count = d7.w (0-112); dst = the destination (a0); d1/d2/d3/a1 pass through
 * Out: dst past the longs; count = -2 x count (the jump's displacement)
 * Evidence: add.w d7,d7 / neg.w d7 / jmp loc_00C9F0(pc,d7.w) into Mem_FillLongsRun; callers $24660
 *           (64 longs of g_HScrollLinesB, 10 of $F526) and $33448 (112 longs of g_HScrollLinesB)
 */
void Mem_FillLongs(uint32_t value, uint32_t d1, uint32_t *d2, uint32_t d3, uint16_t *count, uint32_t *dst,
                   uint32_t *a1)
{
    (void)d1; (void)d2; (void)d3; (void)a1;
    *count = (uint16_t)-(uint16_t)(*count * 2);
    fill_longs(value, dst, units_of(*count, FILLLONGS_UNITS, "Mem_FillLongs"));
}
