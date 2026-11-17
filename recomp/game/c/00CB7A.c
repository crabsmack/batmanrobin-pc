#include "game.h"

/* Mem_FillWords2Run and Mem_FillWords2 ($00CB7A-$00CD85).
 *
 * The original is an unrolled run of `move.w d0,(a0)+ / move.w d2,(a1)+` units entered by
 * `jmp loc_00CD7A(pc,d7.w)` count units before its rts. 128 units are in this function file; the
 * 96 bytes-identical units before it ($00C9FA-$00CB79, raw bytes after Mem_FillLongs) continue the
 * run, so a count up to 224 writes the same pairs (never over 128 in the corpus). The C loops over
 * the same writes, in the same order; a count past 224 would run into Mem_FillLongs' code: a
 * fatal here. */

static constexpr int FILLWORDS2_RUN_UNITS = (0x00CD7A - 0x00CB7A) / 4;   /* 128 */
static constexpr int FILLWORDS2_MAX_UNITS = (0x00CD7A - 0x00C9FA) / 4;   /* 224 */

/* `n` units: the word a to *dst_a, then the word b to *dst_b (68000 addresses), each advanced */
static void fill_pairs(uint16_t a, uint16_t b, uint32_t *dst_a, uint32_t *dst_b, int n)
{
    PTR(u16) pa, pb;
    pa.v = *dst_a;
    pb.v = *dst_b;
    u16 *wa = pa, *wb = pb;
    for (int i = 0; i < n; i++) {
        *wa++ = a;
        *wb++ = b;
    }
    *dst_a = ADDR68(wa);
    *dst_b = ADDR68(wb);
}

/* Mem_FillWords2Run - the unrolled body of Mem_FillWords2 entered at this file's first unit: 128
 * pairs of words, value_a to dst_a and value_b to dst_b.
 * In:  value_a = d0.w, value_b = d2.w; dst_a/dst_b = the destinations (a0/a1)
 * Out: dst_a/dst_b past the words
 * Evidence: 128 x (move.w d0,(a0)+ / move.w d2,(a1)+) then the rts at $00CD7A; entered by the
 *           computed jump at $00CD82 (dkit.toml computed_entry_runs)
 */
void Mem_FillWords2Run(uint16_t value_a, uint16_t value_b, uint32_t *dst_a, uint32_t *dst_b)
{
    fill_pairs(value_a, value_b, dst_a, dst_b, FILLWORDS2_RUN_UNITS);
}

/* Mem_FillWords2 - stores two constant words count times: value_a to dst_a and value_b to dst_b
 * (two line tables at once).
 * In:  value_a = d0.w, value_b = d2.w; count = d7.w; dst_a/dst_b = the destinations (a0/a1)
 * Out: dst_a/dst_b past the words; count = -4 x count (the jump's displacement); value_b as it came
 * Evidence: add.w d7,d7 / add.w d7,d7 / neg.w d7 / jmp loc_00CD7A(pc,d7.w); callers $24900
 *           (stage 2, with the H-int table) and $49AD4 (g_HScrollLinesA: 112 + 112 lines, a
 *           split-screen scroll); 17 corpus runs
 */
void Mem_FillWords2(uint16_t value_a, uint16_t *value_b, uint16_t *count, uint32_t *dst_a, uint32_t *dst_b)
{
    *count = (uint16_t)-(uint16_t)(*count * 4);
    int16_t d = (int16_t)*count;
    if (d > 0 || -d / 4 > FILLWORDS2_MAX_UNITS)
        dk::fatal("Mem_FillWords2: displacement $%04X lands outside the unrolled run", (unsigned)*count);
    fill_pairs(value_a, *value_b, dst_a, dst_b, -d / 4);
}
