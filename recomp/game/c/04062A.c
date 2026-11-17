#include "game.h"

/* The Hatter road's camera and the road itself ($04062A-$040943).
 *
 * The road is plane A drawn with line scroll: Stage3_DrawRoad works out up to 18 checker rows from
 * the bottom of the screen up (each row's depth from Tbl_040C86, its height and sideways offset
 * bent by the road's pitch and bend), and writes each row's lines backwards through the unrolled
 * run Stage3_DrawRoadLinesRun (the next function file: H-scroll into g_HScrollLinesA, V-scroll
 * into the H-int table). The row loop crosses between the two files the way the 68000 does: a row
 * with lines jumps to Stage3_DrawRoadRowLines, and the run's last unit comes back here through the
 * loop's dbra (Stage3_DrawRoadLoopRows -> Stage3_DrawRoadNextRow). Each piece keeps every register
 * the asm keeps, in a RoadRegs. */

/* the row depth table (a word per row, +$20 bytes a row; read at (a2) and -$20(a2)), the depths
   past the last row, and the reciprocal table the line ramps divide by (ROM) */
static constexpr uint32_t Tbl_040C86 = 0x040C86u;
static constexpr uint32_t Tbl_040EB6 = 0x040EB6u;
static constexpr uint32_t Tbl_040ECA = 0x040ECAu;

enum {
    ROW_BYTES = 0x20,              /* the depth table's stride */
    ROWS = 0x11,                   /* d7: 18 rows (dbra) */
    CHECKER_ROW = 0x80,            /* the road's distance per checker row */
    CHECKER_COLOUR = 0x100,        /* the V-scroll offset that swaps the checker colours */
    HORIZON_Y = 0x64,              /* the horizon line before the shake */
    SPIN_STEPS = 0xF0,             /* the thrown toys' 60 rotation steps x 4 */
    ORIGIN_X = 0xC0, ORIGIN_Y = 0x1F,   /* the road objects' screen origin */
    SCREEN_LINE_BASE = 0x48 + 0xDF,     /* the first row's line: $48 + 223 - the horizon */
    HEIGHT_BIAS = 0x240,
    LAST_ROW_BEND = 0x2200,        /* the flat stretch past the last row: bend x $2200 */
    LAST_ROW_PITCH = 0x31C,        /* and pitch x $31C */
    LINE_ORIGIN = 0xA0,            /* the H-scroll's centre */
};
static constexpr uint32_t FIRST_LINES = 0x00E000E0u;     /* d6: both line counters start at line 224 */
static constexpr int32_t PITCH_MIN = (int32_t)0xFE200000, PITCH_MAX = 0x280000;
static constexpr int32_t BEND_MIN = (int32_t)0xFFE80000, BEND_MAX = 0x180000;
static constexpr uint32_t CAMERA_FIXED_X = 0x14A0000u;   /* g_PlayMode bit $800: the camera aims here */
static constexpr int32_t CAMERA_SPEED = 0x30000;         /* at most 3 px a frame */
static constexpr int32_t CAMERA_MIN_X = 0x480000, CAMERA_MAX_X = 0x1100000;
static constexpr uint32_t HALF_SCREEN = 160u << 16;     /* $A00000: 160 px, 16.16 */

/* the registers the road loop keeps, as the asm keeps them */
struct RoadRegs {
    uint32_t d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6;
    dk::Flags cc;
};

static uint32_t swap(uint32_t v) { return (v << 16) | (v >> 16); }
static uint16_t lo(uint32_t v) { return (uint16_t)v; }
static uint16_t hi(uint32_t v) { return (uint16_t)(v >> 16); }
static void set_w(uint32_t &r, uint16_t v) { r = (r & 0xFFFF0000u) | v; }
static uint32_t muls(uint32_t a, uint32_t b) { return (uint32_t)((int32_t)(int16_t)a * (int32_t)(int16_t)b); }
static uint32_t mulu(uint32_t a, uint32_t b) { return (uint32_t)(uint16_t)a * (uint16_t)b; }
static uint32_t ror(uint32_t v, int n) { return (v >> n) | (v << (32 - n)); }
static uint32_t ext(uint32_t v) { return (uint32_t)(int32_t)(int16_t)v; }
static uint16_t rom_w(uint32_t a) { PTR(const u16) p; p.v = a; return *p; }
static void put_w(uint32_t &a, uint16_t v) { PTR(u16) p; p.v = a; *p = v; a += 2; }
static void put_l(uint32_t &a, uint32_t v) { PTR(u32) p; p.v = a; *p = v; a += 4; }
/* dbra: the low word counts down; true = branch (not past -1) */
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

/* Stage3_DrawRoadNextRow's body ($040830): the row just drawn into the row table (its two line
   counters, x offset and depth), the checker colour swapped, then the next row's bend and pitch
   slopes; past the last row the flat stretch to the horizon (Tbl_040EB6) */
static void next_row(RoadRegs &r)
{
    HatterRoadVars *road = hatter_road();
    road->colour = (uint16_t)(road->colour ^ CHECKER_COLOUR);
    put_l(r.a4, r.d6);
    put_w(r.a4, hi((uint32_t)road->x_offset));
    put_w(r.a4, rom_w(r.a2));
    if (lo(r.d7) == 0) {
        r.a2 = Tbl_040EB6;
        r.d0 = muls((uint32_t)(road->bend >> 6), LAST_ROW_BEND) + (uint32_t)road->scroll_x;
        road->x_offset = (int32_t)r.d0;
        r.d0 = (uint32_t)road->pitch;
        bool negative = (int32_t)r.d0 < 0;
        if (negative)
            r.d0 = (uint32_t)-(int32_t)r.d0;
        r.d1 = r.d0;
        r.d0 = mulu(swap(r.d0), LAST_ROW_PITCH);   /* the integer part x $31C ... */
        r.d1 = mulu(r.d1, LAST_ROW_PITCH);          /* ... and the fraction */
        r.d0 = (swap(r.d0) & 0xFFFF0000u) + r.d1;   /* swap / clr.w / add.l */
        if (negative)
            r.d0 = (uint32_t)-(int32_t)r.d0;
        r.d1 = swap(ext((uint32_t)(uint16_t)road->horizon) << 3);
        r.d0 += r.d1;
        road->height = (int32_t)r.d0;
        return;
    }
    r.a2 += ROW_BYTES;
    r.d0 = (uint32_t)road->bend + (uint32_t)road->bend_acc;
    road->bend_acc = (int32_t)r.d0;
    road->x_offset = (int32_t)((uint32_t)road->x_offset + r.d0);
    r.d0 = (uint32_t)road->pitch_acc + (uint32_t)road->pitch;
    r.d1 = (uint32_t)((int32_t)r.d0 >> 2);
    r.d0 += r.d1;
    road->pitch_acc = (int32_t)r.d0;
    road->height = (int32_t)((uint32_t)road->height + r.d0);
}

enum RowExit { ROW_EMPTY, ROW_HAS_LINES, ROW_CLIPPED };

/* Stage3_DrawRoadRow's body ($0408C8): one row: its depth, the x offset and height scaled by it,
   its line count (the height step from the line reached so far); ROW_EMPTY = no lines (the loop's
   dbra in the next file), ROW_HAS_LINES = the ramps are set for Stage3_DrawRoadRowLines,
   ROW_CLIPPED = the row ends above the screen (its own dbra, below) */
static RowExit row(RoadRegs &r)
{
    HatterRoadVars *road = hatter_road();
    set_w(r.d3, rom_w(r.a2));
    r.d0 = (uint32_t)road->x_scaled;
    set_w(r.d1, hi((uint32_t)road->x_offset));
    r.d1 = (uint32_t)((int32_t)muls(r.d1, r.d3) >> 6);
    road->x_scaled = (int32_t)r.d1;
    set_w(r.d2, road->depth);
    road->depth = lo(r.d3);
    set_w(r.d4, hi((uint32_t)road->height));
    r.d4 = muls(r.d4, r.d3);
    r.d4 = swap(r.d4 + r.d4);
    set_w(r.d4, (uint16_t)(lo(r.d4) - (uint16_t)road->line));
    road->line = (int16_t)(road->line + lo(r.d4));
    r.d6 = swap(r.d6);
    set_w(r.d6, (uint16_t)(lo(r.d6) + lo(r.d4)));
    r.d6 = swap(r.d6);
    /* neg.w d4 / ble.w: no lines (a height step of $8000 overflows and counts as lines) */
    uint16_t lines = (uint16_t)-lo(r.d4);
    bool overflow = lo(r.d4) == 0x8000;
    set_w(r.d4, lines);
    if (lines == 0 || (((int16_t)lines < 0) != overflow))
        return ROW_EMPTY;
    set_w(r.d5, (uint16_t)(lines * 2));
    set_w(r.d5, rom_w(r.a3 + ext(r.d5)));          /* 1 / lines */
    r.d1 = muls(r.d1 - r.d0, r.d5);                 /* the x offset's step a line */
    r.d0 = ror(r.d0, 6);
    set_w(r.d0, (uint16_t)(lo(r.d0) + LINE_ORIGIN));
    set_w(r.d3, (uint16_t)(lo(r.d3) - lo(r.d2)));
    r.d3 = (muls(r.d3, r.d5) << 2) + 0x10000u;      /* the V-scroll's step a line */
    r.d2 = ror(ext(r.d2), 4);
    set_w(r.d2, (uint16_t)(lo(r.d2) - road->colour - lo(r.d6)));
    /* sub.w d4,d6 / bpl.w: the row fits on the screen */
    uint16_t left = (uint16_t)(lo(r.d6) - lines);
    set_w(r.d6, left);
    if ((int16_t)left >= 0)
        return ROW_HAS_LINES;
    /* add.w d6,d4 / bgt.w: the lines that still fit */
    uint16_t fit = (uint16_t)(lines + left);
    bool v = ((lines ^ fit) & (left ^ fit) & 0x8000) != 0;
    set_w(r.d4, fit);
    if (fit != 0 && (((int16_t)fit < 0) == v))
        return ROW_HAS_LINES;
    return ROW_CLIPPED;
}

/* the row loop from Stage3_DrawRoadRow: rows without lines that end above the screen loop here;
   a row with lines or an empty one leaves for the next file (the run, or the loop's dbra) */
static void from_row(RoadRegs &r, uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                     uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a4, uint32_t *a5,
                     uint32_t *a6, dk::Flags *ccr)
{
    for (;;) {
        RowExit exit = row(r);
        if (exit == ROW_EMPTY) {
            store(r, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a4, a5, a6, ccr);
            dk::jump(at_Stage3_DrawRoadLoopRows);
            return;
        }
        if (exit == ROW_HAS_LINES) {
            store(r, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a4, a5, a6, ccr);
            dk::jump(at_Stage3_DrawRoadRowLines);
            return;
        }
        r.d6 = ext(r.d6);
        if (dbra(r.d7)) {
            next_row(r);
            continue;
        }
        /* the last row's entry, then rts (Stage3_DrawRoadDone): nothing above to clear */
        put_l(r.a4, r.d6);
        put_w(r.a4, hi((uint32_t)hatter_road()->x_offset));
        uint16_t depth = rom_w(r.a2);
        put_w(r.a4, depth);
        r.cc = dk::Flags::test(depth, 2, r.cc.x);
        store(r, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a4, a5, a6, ccr);
        return;
    }
}

/* Stage3_RoadCameraFollow - the Hatter road's camera: aims g_CameraX at the players' x (the mean
 * of both in play; x $14A with g_PlayMode bit $800) minus 160, at most 3 px a frame, kept within
 * $48-$110, stores -(x * 9/8) as the road's sideways scroll, then Camera_UpdateView; with no
 * player in play only Camera_UpdateView
 * In:  g_PlayMode, g_P1Flags/g_P2Flags bit 0, the players' x
 * Out: g_CameraX, the road's scroll_x; d0/d1 as left (no player: d0 = player 2's x), then
 *      Camera_UpdateView's
 * Evidence: move.w g_PlayMode / andi.w #$800 / #$14A0000; btst #0 g_P1Flags / g_Player1+$2E / btst
 *           g_P2Flags / lsr.l #1 both / add.l; subi.l #$A00000 / sub.l g_CameraX / the +-$30000
 *           clamp / add.l / the $480000-$1100000 clamp / asr.l #3 / add.l / neg.l / move.l
 *           g_SceneVars2+$12 / jmp Camera_UpdateView; Op2_CallNativeRelSaved at $04061E in the road
 *           driver Scr2_04061A, every frame; TAS f113312-f116145
 */
void Stage3_RoadCameraFollow(uint32_t *d0, uint32_t *d1, uint32_t *d2)
{
    uint32_t x;
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)(g_PlayMode & 0x800);
    if (g_PlayMode & 0x800) {
        x = CAMERA_FIXED_X;
    } else if (g_P1Flags & 1) {
        x = ((uint32_t)(uint16_t)((const PlayerRec *)g_Player1)->x << 16) | ((const PlayerRec *)g_Player1)->x_frac;
        if (g_P2Flags & 1) {
            *d1 = ((uint32_t)(uint16_t)((const PlayerRec *)g_Player2)->x << 16) | ((const PlayerRec *)g_Player2)->x_frac;
            x = (x >> 1) + (*d1 >> 1);
        }
    } else {
        x = ((uint32_t)(uint16_t)((const PlayerRec *)g_Player2)->x << 16) | ((const PlayerRec *)g_Player2)->x_frac;
        if (!(g_P2Flags & 1)) {
            *d0 = x;
            dk::jump(at_Camera_UpdateView);
            return;
        }
    }
    int32_t move = (int32_t)(x - HALF_SCREEN - g_CameraX);
    if (move >= 0) {
        if (move > CAMERA_SPEED)
            move = CAMERA_SPEED;
    } else if (move < -CAMERA_SPEED) {
        move = -CAMERA_SPEED;
    }
    int32_t camera = (int32_t)((uint32_t)move + g_CameraX);
    if (camera < CAMERA_MIN_X)
        camera = CAMERA_MIN_X;
    else if (camera > CAMERA_MAX_X)
        camera = CAMERA_MAX_X;
    g_CameraX = (uint32_t)camera;
    *d1 = (uint32_t)(camera >> 3);
    *d0 = (uint32_t)-(int32_t)((uint32_t)camera + *d1);
    hatter_road()->scroll_x = (int32_t)*d0;
    dk::jump(at_Camera_UpdateView);
}

/* Stage3_DrawRoad - the Hatter road's per-frame update and drawing: clamps the bend (+-24 px) and
 * pitch, moves the road by its speed (distance, checker phase), steps the thrown toys' spin,
 * sets the horizon ($64 + shake) and the road objects' screen origin; then draws the road: the
 * first row's entry, then up to 18 rows through the row loop
 * In:  the road state (hatter_road()), set by the boss's road steerer
 * Out: the road state; the row table at g_SceneVars+$12 (8 bytes a row: two line counters, x offset,
 *      depth) for the road objects; g_HScrollLinesA and the H-int table g_HIntTblBack written
 *      backwards from line 223; the lines above the road cleared (Span_ClearLines); every
 *      register as the row loop leaves it
 * Evidence: the clamps cmpi.l #$FE200000/#$280000 and #$FFE80000/#$180000; move.w g_SceneVars2+8 /
 *           +$C / add.w +$10 / andi.w #$7F / +$E; #$64 + g_ShakeY; +$3E +4 mod $F0; #$C0 -
 *           g_SprOffsetX, #$1F - g_SprOffsetY; lea g_HScrollLinesB,a0 / g_HIntTblBack + $1C0,a1 /
 *           Tbl_040C86-$20,a2 / Tbl_040ECA,a3 / g_SceneVars+$12,a4; the first entry (#$48 + #$DF -
 *           horizon); #$E000E0,d6 / #$11,d7 / bra.w the row; Op2_CallNativeRelSaved at $040622 in
 *           Scr2_04061A, every frame; TAS f113312-f116145 (png f113400-f115800: the checkered road
 *           bends and dips)
 */
void Stage3_DrawRoad(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                     uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5,
                     uint32_t *a6, dk::Flags *ccr)
{
    HatterRoadVars *road = hatter_road();
    RoadRegs r;
    load(r, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, *a3, a4, a5, a6, ccr);

    int32_t pitch = road->pitch;
    pitch = pitch < PITCH_MIN ? PITCH_MIN : pitch > PITCH_MAX ? PITCH_MAX : pitch;
    road->pitch = pitch;
    int32_t bend = road->bend;
    bend = bend < BEND_MIN ? BEND_MIN : bend > BEND_MAX ? BEND_MAX : bend;
    road->bend = bend;
    r.d0 = (uint32_t)bend;
    set_w(r.d0, hi((uint32_t)road->speed));
    road->step = (int16_t)lo(r.d0);
    set_w(r.d0, (uint16_t)(lo(r.d0) + road->distance));
    road->distance = lo(r.d0);
    set_w(r.d0, (uint16_t)(lo(r.d0) & (CHECKER_ROW - 1)));
    road->phase = lo(r.d0);
    set_w(r.d0, (uint16_t)(HORIZON_Y + hi(g_ShakeY)));
    road->horizon = (int16_t)lo(r.d0);
    uint16_t spin = (uint16_t)(road->spin + 4);
    if ((int16_t)spin >= SPIN_STEPS)
        spin = (uint16_t)(spin - SPIN_STEPS);
    road->spin = spin;
    road->origin_x = (int16_t)(ORIGIN_X - g_SprOffsetX);
    set_w(r.d0, (uint16_t)(ORIGIN_Y - g_SprOffsetY));
    road->origin_y = (int16_t)lo(r.d0);

    /* the row loop's tables: lines written backwards from the bottom of both line tables */
    r.a0 = dk::addr(g_HScrollLinesB);              /* the end of g_HScrollLinesA */
    r.a1 = ext(g_HIntTblBack) + 0x1C0;
    r.a2 = Tbl_040C86 - ROW_BYTES;
    r.a3 = Tbl_040ECA;
    r.a4 = dk::addr(g_SceneVars + offsetof(HatterRoadScene, rows));
    road->depth = 0x1000;
    r.d0 = (uint32_t)(uint16_t)road->horizon;
    road->line = (int16_t)lo(r.d0);
    r.d0 = swap(r.d0) << 3;
    road->height = (int32_t)r.d0;
    r.d0 = (uint32_t)road->scroll_x;
    road->x_offset = (int32_t)r.d0;
    r.d0 = ext(swap(r.d0 << 6));
    road->x_scaled = (int32_t)r.d0;
    set_w(r.d0, (uint16_t)((road->distance & CHECKER_ROW) * 2));
    road->colour = lo(r.d0);
    /* the checker phase picks where in the depth table the first row starts */
    set_w(r.d0, (uint16_t)(road->phase & 0x78));
    set_w(r.d1, lo(r.d0));
    set_w(r.d0, (uint16_t)((0x28 - (lo(r.d0) >> 3)) * 2));
    r.a2 += ext(r.d0);
    r.d0 = muls(swap((uint32_t)road->pitch << 6), r.d1);
    r.d0 = (uint32_t)-(int32_t)(r.d0 << 3);
    road->pitch_acc = (int32_t)r.d0;
    r.d0 = (uint32_t)-(int32_t)muls((uint32_t)(road->bend >> 7), r.d1);
    road->bend_acc = (int32_t)r.d0;
    set_w(r.d0, rom_w(r.a2 - ROW_BYTES));
    set_w(r.d1, (uint16_t)(hi((uint32_t)road->height) - HEIGHT_BIAS));
    r.d1 = muls(r.d1, r.d0);
    r.d1 = swap(r.d1 + r.d1);
    set_w(r.d1, (uint16_t)(lo(r.d1) + SCREEN_LINE_BASE - road->horizon));
    put_w(r.a4, lo(r.d1));
    put_w(r.a4, lo(r.d1));
    put_w(r.a4, hi((uint32_t)road->x_offset));
    put_w(r.a4, lo(r.d0));
    r.d6 = FIRST_LINES;
    set_w(r.d7, ROWS);
    *a3 = r.a3;
    from_row(r, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a4, a5, a6, ccr);
}

/* Stage3_DrawRoadNextRow - the row loop's head: the row just drawn into the row table, the checker
 * colour swapped, the next row's slopes (or the flat stretch past the last row), then the row
 * In:  the row loop's registers (a2 depth table, a3 reciprocals, a4 row table, d6 the two line
 *      counters, d7 rows left)
 * Out: as the row loop leaves them
 * Evidence: eori.w #$100,g_SceneVars2+$20 / move.l d6,(a4)+ / move.w +$32,(a4)+ / move.w (a2),(a4)+ /
 *           tst.w d7 / bne; lea Tbl_040EB6,a2 / asr.l #6 / muls.w #$2200 / the mulu.w #$31C pair;
 *           lea $20(a2) / the bend and pitch slopes; the dbra target of Stage3_DrawRoadLoopRows and
 *           of the row's own tail; TAS f113312-f116145
 */
void Stage3_DrawRoadNextRow(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                            uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a3,
                            uint32_t *a4, uint32_t *a5, uint32_t *a6, dk::Flags *ccr)
{
    RoadRegs r;
    load(r, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6, ccr);
    next_row(r);
    from_row(r, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a4, a5, a6, ccr);
}

/* Stage3_DrawRoadRow - one road row: its depth, x offset and height, its line count; a row with
 * lines continues in Stage3_DrawRoadRowLines, an empty one at the loop's dbra
 * In:  the row loop's registers
 * Out: as the row loop leaves them
 * Evidence: move.w (a2),d3 / muls.w d3 of +$32 and +$2A / asr.l #6 / sub.w +$22 / swap d6 / neg.w d4
 *           / ble.w / move.w (a3,d5.w),d5 / ror.l #6,d0 / addi.w #$A0 / asl.l #2,d3 / addi.l
 *           #$10000 / ror.l #4,d2 / sub.w d4,d6 / bpl.w / add.w d6,d4 / bgt.w / ext.l d6 / dbra;
 *           bra.w from Stage3_DrawRoad's setup; TAS f113312-f116145
 */
void Stage3_DrawRoadRow(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                        uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t *a4,
                        uint32_t *a5, uint32_t *a6, dk::Flags *ccr)
{
    RoadRegs r;
    load(r, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6, ccr);
    from_row(r, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a4, a5, a6, ccr);
}

/* Stage3_DrawRoadDone - the road drawing's rts when the rows reach the top of the screen (nothing
 * above them to clear)
 * In:  -
 * Out: -
 * Evidence: rts at $040942; ble.w from $040C52 (d6 <= 0)
 */
void Stage3_DrawRoadDone(void)
{
}
