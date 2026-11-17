#include "game.h"

/* The Hatter road's line writer ($040980-$040C85): the unrolled run and the row loop's tail.
 *
 * The original writes a row's lines with an unrolled run of 59 twelve-byte units (`move.w d0,-(a0)`
 * / `move.w d2,-(a1)`: one line's H-scroll and V-scroll going up both tables, then both 16.16 ramps
 * stepped with add.w / addx.w), entered by `jmp loc_040C44(pc,d7.w)` at $040C82 with d7 = -12 x
 * the row's lines. The last unit swaps d7 back and falls into the row loop's dbra. The C loops over
 * the same writes in the same order. The 5 identical units before the run ($040944-$04097F) are
 * raw: no row ever had more than 59 lines in the corpus (dkit.toml computed_entry_runs). */

/* units in the unrolled run $040980-$040C43 */
static constexpr int RUN_UNITS = (0x040C44 - 0x040980) / 12;

/* the registers the road loop keeps, as the asm keeps them */
struct RoadRegs {
    uint32_t d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6;
    dk::Flags cc;
};

static uint32_t swap(uint32_t v) { return (v << 16) | (v >> 16); }
static uint16_t lo(uint32_t v) { return (uint16_t)v; }
static void set_w(uint32_t &r, uint16_t v) { r = (r & 0xFFFF0000u) | v; }
static uint32_t ext(uint32_t v) { return (uint32_t)(int32_t)(int16_t)v; }
static uint16_t rom_w(uint32_t a) { PTR(const u16) p; p.v = a; return *p; }
static void put_w(uint32_t &a, uint16_t v) { PTR(u16) p; p.v = a; *p = v; a += 2; }
static void put_l(uint32_t &a, uint32_t v) { PTR(u32) p; p.v = a; *p = v; a += 4; }
static void push_w(uint32_t &a, uint16_t v) { a -= 2; PTR(u16) p; p.v = a; *p = v; }
static bool dbra(uint32_t &d) { set_w(d, (uint16_t)(d - 1)); return lo(d) != 0xFFFF; }

static void load(RoadRegs &r, uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                 uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t *a4,
                 uint32_t *a5, uint32_t *a6, const dk::Flags *ccr)
{
    r = RoadRegs{ *d0, *d1, *d2, *d3, *d4, *d5, *d6, *d7, *a0, *a1, *a2, a3, *a4, *a5, *a6, *ccr };
}
static void store(const RoadRegs &r, uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4,
                  uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a4,
                  uint32_t *a5, uint32_t *a6, dk::Flags *ccr)
{
    *d0 = r.d0; *d1 = r.d1; *d2 = r.d2; *d3 = r.d3; *d4 = r.d4; *d5 = r.d5; *d6 = r.d6; *d7 = r.d7;
    *a0 = r.a0; *a1 = r.a1; *a2 = r.a2; *a4 = r.a4; *a5 = r.a5; *a6 = r.a6; *ccr = r.cc;
}

/* `n` units of the run: n lines written upward (H-scroll d0 into -(a0), V-scroll d2 into -(a1)),
   the ramps stepped between them (d4 += a5 carrying into d0 += d1; d5 += a6 into d2 += d3), then
   the last unit's swap d7 */
static void write_lines(RoadRegs &r, int n)
{
    for (int i = 0; i < n; i++) {
        push_w(r.a0, lo(r.d0));
        push_w(r.a1, lo(r.d2));
        if (i == n - 1)
            break;
        uint32_t sum = (uint32_t)lo(r.d4) + lo(r.a5);              /* add.w a5,d4 */
        set_w(r.d4, (uint16_t)sum);
        set_w(r.d0, (uint16_t)(lo(r.d0) + lo(r.d1) + (sum >> 16)));  /* addx.w d1,d0 */
        sum = (uint32_t)lo(r.d5) + lo(r.a6);                       /* add.w a6,d5 */
        set_w(r.d5, (uint16_t)sum);
        set_w(r.d2, (uint16_t)(lo(r.d2) + lo(r.d3) + (sum >> 16)));  /* addx.w d3,d2 */
    }
    r.d7 = swap(r.d7);
}

/* the row loop's dbra ($040C3E) and, after the last row, its tail ($040C42-$040C84): the last
   row's entry, then the lines above the road cleared; the next row is Stage3_DrawRoadNextRow (the
   previous file) */
static void loop_rows(RoadRegs &r, uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4,
                      uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a4,
                      uint32_t *a5, uint32_t *a6, dk::Flags *ccr)
{
    if (dbra(r.d7)) {
        store(r, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a4, a5, a6, ccr);
        dk::jump(at_Stage3_DrawRoadNextRow);
        return;
    }
    put_l(r.a4, r.d6);
    put_w(r.a4, (uint16_t)((uint32_t)hatter_road()->x_offset >> 16));
    put_w(r.a4, rom_w(r.a2));
    r.a0 = dk::addr(g_HScrollLinesA);
    r.a1 = ext(g_HIntTblBack);
    set_w(r.d7, lo(r.d6));                       /* the lines left above the road */
    r.cc = dk::Flags::test(lo(r.d6), 2, r.cc.x);
    store(r, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a4, a5, a6, ccr);
    if ((int16_t)lo(r.d6) <= 0) {
        dk::jump(at_Stage3_DrawRoadDone);
        return;
    }
    *d0 = 0;
    *d2 = 0;
    dk::jump(at_Span_ClearLines);
}

/* Stage3_DrawRoadLinesRun - the unrolled line writer entered at its first unit: a row of 59 lines
 * (the run's whole length), then the row loop's dbra
 * In:  a0/a1 = the line tables (H-scroll, H-int V-scroll; written upward), d0/d2 = the H/V
 *      scroll, d1/d3 = their integer steps, a5/a6 = their fraction steps, d4/d5 = the fractions;
 *      d7 = rows left (swapped)
 * Out: a0/a1 moved up 59 lines; then as the row loop leaves the registers
 * Evidence: 58 x (move.w d0,-(a0) / move.w d2,-(a1) / add.w a5,d4 / addx.w d1,d0 / add.w a6,d5 /
 *           addx.w d3,d2), then move.w d0,-(a0) / move.w d2,-(a1) / swap d7 and the dbra at $040C3E;
 *           entered only by the computed jump at $040C82 (dkit.toml computed_entry_runs), so its
 *           start is that jump's largest count; the unit starts are ctargets in fuzz-113400/115200
 *           and the TAS
 */
void Stage3_DrawRoadLinesRun(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                             uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                             uint32_t *a4, uint32_t *a5, uint32_t *a6, dk::Flags *ccr)
{
    RoadRegs r;
    load(r, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, *a3, a4, a5, a6, ccr);
    write_lines(r, RUN_UNITS);
    loop_rows(r, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a4, a5, a6, ccr);
}

/* Stage3_DrawRoadLoopRows - the dbra of the road's row loop: the next row (Stage3_DrawRoadNextRow),
 * or after the last one its row-table entry and the lines above the horizon cleared
 * (Span_ClearLines from line 0, d7 = the lines left); none left: rts
 * In:  d7 = rows left, d6 = the two line counters (low word: the lines left above), a2 = the depth
 *      table, a4 = the row table
 * Out: the row table's last entry; a0 = g_HScrollLinesA, a1 = the H-int table, d7's low word = the
 *      lines left, the condition codes of that move; then Span_ClearLines's (d0 = d2 = 0)
 * Evidence: dbra d7,Stage3_DrawRoadNextRow / move.l d6,(a4)+ / move.w g_SceneVars2+$32,(a4)+ /
 *           move.w (a2),(a4)+ / lea g_HScrollLinesA,a0 / movea.w g_HIntTblBack,a1 / move.w d6,d7 /
 *           ble.w Stage3_DrawRoadDone / moveq #0,d0 / moveq #0,d2 / jmp Span_ClearLines; ble.w from
 *           Stage3_DrawRoadRow; TAS f113312-f116145
 */
void Stage3_DrawRoadLoopRows(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                             uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a3,
                             uint32_t *a4, uint32_t *a5, uint32_t *a6, dk::Flags *ccr)
{
    RoadRegs r;
    load(r, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6, ccr);
    loop_rows(r, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a4, a5, a6, ccr);
}

/* Stage3_DrawRoadRowLines - a road row with lines to draw: turns its ramps into the run's
 * registers (fractions in d4/d5, integer steps in d1/d3, fraction steps in a5/a6) and writes its
 * lines (the jump 12 bytes a line before the run's end), then the row loop's dbra
 * In:  d4's low word = the row's lines (1-59), d0-d3 the ramps (fraction in the high words), d7 =
 *      rows left
 * Out: d7 = -12 x lines while the lines are written, swapped back after; then as the row loop
 *      leaves the registers
 * Evidence: swap d7 / add.w d4,d4 twice / move.w d4,d7 / add.w d4,d4 / add.w d4,d7 / neg.w d7 /
 *           move.l d0,d4 / swap d4 / movea.w d1,a5 / swap d1 / move.l d2,d5 / swap d5 / movea.w
 *           d3,a6 / swap d3 / jmp loc_040C44(pc,d7.w) at $040C82; bpl.w / bgt.w targets in
 *           Stage3_DrawRoadRow; TAS f113312-f116145
 */
void Stage3_DrawRoadRowLines(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                             uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a3,
                             uint32_t *a4, uint32_t *a5, uint32_t *a6, dk::Flags *ccr)
{
    RoadRegs r;
    load(r, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6, ccr);
    r.d7 = swap(r.d7);
    int16_t lines = (int16_t)lo(r.d4);
    set_w(r.d4, (uint16_t)(lo(r.d4) * 4));
    set_w(r.d7, lo(r.d4));
    set_w(r.d4, (uint16_t)(lo(r.d4) * 2));
    set_w(r.d7, (uint16_t)-(uint16_t)(lo(r.d7) + lo(r.d4)));   /* -12 x lines: the jump's displacement */
    r.d4 = swap(r.d0);
    r.a5 = ext(r.d1);
    r.d1 = swap(r.d1);
    r.d5 = swap(r.d2);
    r.a6 = ext(r.d3);
    r.d3 = swap(r.d3);
    if (lines < 1 || lines > RUN_UNITS)
        dk::fatal("road row of %d lines: outside the unrolled run (the 5 raw units before $040980)", lines);
    write_lines(r, lines);
    loop_rows(r, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a4, a5, a6, ccr);
}
