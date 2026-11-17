#include "game.h"

/* Coll_WalkBlockMap and its loops ($0120A2-$012571): the grounded move through plane A's 16x16
 * block collision map (docs/NOTES.md "Terrain collision"), with the cursor of Coll_MoveBlockMap
 * (011D5E.c): d4/d5 high words the block column / row, low words the pixel (x as pixel x 2); a3 the
 * block's map word, a0 its collision data, a6 = g_LayerA. Every exit is Coll_BlockMapResult. */

/* the registers as the 68000 has them through the walk */
struct Regs {
    uint32_t d[8];
    uint32_t a[7];
};

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }
static uint32_t swap(uint32_t v) { return (v << 16) | (v >> 16); }
static uint32_t sext(uint16_t w) { return (uint32_t)(int32_t)(int16_t)w; }

static const MapLayer *layer(const Regs &r)
{
    PTR(const MapLayer) p;
    p.v = r.a[6];
    return p;
}

static uint16_t word_at(uint32_t a)
{
    PTR(const u16) p;
    p.v = a;
    return *p.get();
}

static uint8_t byte_at(uint32_t a)
{
    PTR(const u8) p;
    p.v = a;
    return *p.get();
}

/* the block under a3 becomes current: its attribute word to g_CollBlockAttr, a0 = its collision
 * data (move.w (a3),d0 / lsr.w #1,d0 / movea.l $14(a6),a0 / move.w 2(a0,d0.w),g_CollBlockAttr /
 * movea.w (a0,d0.w),a0 / adda.l $18(a6),a0) */
static void load_block(Regs &r)
{
    const MapLayer *L = layer(r);
    set_low_word(&r.d[0], (uint16_t)(word_at(r.a[3]) >> 1));
    uint32_t entry = L->block_table.raw() + sext((uint16_t)r.d[0]);
    g_CollBlockAttr = word_at(entry + 2);
    r.a[0] = sext(word_at(entry)) + L->collision.raw();
}

/* one pixel right: into the next block when the offset wraps (and the column after it is inside
   the map; the column count moves on regardless) */
static void x_next(Regs &r)
{
    set_low_word(&r.d[4], (uint16_t)((r.d[4] + 2) & 0x1E));
    if ((uint16_t)r.d[4] != 0)
        return;
    r.d[4] = swap(r.d[4]);
    int16_t col = (int16_t)(uint16_t)r.d[4];
    set_low_word(&r.d[4], (uint16_t)(col + 1));
    if ((int32_t)col + 1 > 0 && (int16_t)(uint16_t)r.d[4] < (int16_t)layer(r)->width) {
        r.a[3] += 2;
        load_block(r);
    }
    r.d[4] = swap(r.d[4]);
}

/* one pixel left: into the previous block when the offset borrows (loaded when the current
   column is inside the map, 1..width-1) */
static void x_prev(Regs &r)
{
    bool borrow = (uint16_t)r.d[4] < 2;
    set_low_word(&r.d[4], (uint16_t)(r.d[4] - 2));
    if (!borrow)
        return;
    set_low_word(&r.d[4], (uint16_t)(r.d[4] & 0x1E));
    r.d[4] = swap(r.d[4]);
    int16_t col = (int16_t)(uint16_t)r.d[4];
    if (!(col >= (int16_t)layer(r)->width) && !(col <= 0)) {
        r.a[3] -= 2;
        load_block(r);
    }
    set_low_word(&r.d[4], (uint16_t)(col - 1));
    r.d[4] = swap(r.d[4]);
}

/* one pixel down (the row after the block's last is the next block's first) */
static void y_next(Regs &r)
{
    set_low_word(&r.d[5], (uint16_t)((r.d[5] + 1) & 0xF));
    if ((uint16_t)r.d[5] != 0)
        return;
    r.d[5] = swap(r.d[5]);
    int16_t row = (int16_t)(uint16_t)r.d[5];
    set_low_word(&r.d[5], (uint16_t)(row + 1));
    if ((int32_t)row + 1 > 0 && (int16_t)(uint16_t)r.d[5] < (int16_t)layer(r)->height) {
        r.a[3] += sext((uint16_t)layer(r)->map_row_bytes);
        load_block(r);
    }
    r.d[5] = swap(r.d[5]);
}

/* one pixel up */
static void y_prev(Regs &r)
{
    bool borrow = (uint16_t)r.d[5] < 1;
    set_low_word(&r.d[5], (uint16_t)(r.d[5] - 1));
    if (!borrow)
        return;
    set_low_word(&r.d[5], (uint16_t)(r.d[5] & 0xF));
    r.d[5] = swap(r.d[5]);
    int16_t row = (int16_t)(uint16_t)r.d[5];
    if (!(row >= (int16_t)layer(r)->height) && !(row <= 0)) {
        r.a[3] -= sext((uint16_t)layer(r)->map_row_bytes);
        load_block(r);
    }
    set_low_word(&r.d[5], (uint16_t)(row - 1));
    r.d[5] = swap(r.d[5]);
}

/* the cursor's start on one axis (move.w pos,d0 / swap pos / asr.w #4,d0 / move.w d0,pos / bmi /
   cmp.w size,pos / bcs / move.w size,d0 / subq.w #1,d0): as Coll_MoveBlockMap's, but only a
   negative index leaves the map pointer (bmi, not ble) */
static bool cursor_start(Regs &r, uint32_t *pos, uint16_t size)
{
    set_low_word(&r.d[0], (uint16_t)*pos);
    *pos = swap(*pos);
    set_low_word(&r.d[0], (uint16_t)((int16_t)(uint16_t)r.d[0] >> 4));
    set_low_word(pos, (uint16_t)r.d[0]);
    if ((int16_t)(uint16_t)r.d[0] < 0)
        return false;
    if (!((uint16_t)r.d[0] < size))
        set_low_word(&r.d[0], (uint16_t)(size - 1));
    return true;
}

/* the column test alone: the cursor's pixel row inside the block's [top, bottom] pair for its
   column (cmp.b 2(a0,d4.w),d5 / bcs / cmp.b 3(a0,d4.w),d5 / bgt: top unsigned, bottom signed) */
static bool in_column(const Regs &r)
{
    uint32_t column = r.a[0] + sext((uint16_t)r.d[4]);
    uint8_t y = (uint8_t)r.d[5];
    if (y < byte_at(column + 2))
        return false;
    if ((int8_t)y > (int8_t)byte_at(column + 3))
        return false;
    return true;
}

/* is the cursor's pixel solid for a mask? d0 low word = the block's mask & it (move.w (a0),d0 /
   and.w mask,d0 / beq / the column test) */
static bool solid(Regs &r, uint16_t mask)
{
    set_low_word(&r.d[0], (uint16_t)(word_at(r.a[0]) & mask));
    return (uint16_t)r.d[0] != 0 && in_column(r);
}

/* dbra on a register's low word: true while it hasn't run out (-1) */
static bool dbra(uint32_t *r)
{
    set_low_word(r, (uint16_t)(*r - 1));
    return (uint16_t)*r != 0xFFFFu;
}

/* how a walk loop ended: blocked by a wall (on to the probe), or its pixels used up (the end) */
enum WalkExit { WALK_BLOCKED, WALK_DONE };

/* the walk loops (Coll_WalkBlockMapRight / Left): per pixel sideways, a wall at the walker's
   height is climbed when the pixel above is free (else the walk stops there, both steps undone);
   with no wall, the walker follows the floor down: one pixel when there is floor there, else two */
static WalkExit walk(Regs &r, bool right)
{
    do {
        if (right)
            x_next(r);
        else
            x_prev(r);
        if (solid(r, (uint16_t)r.d[6])) {
            y_prev(r);
            if (solid(r, (uint16_t)r.d[6])) {   /* a wall: back to where the step began */
                set_low_word(&r.d[6], (uint16_t)r.d[0]);
                if (right)
                    x_prev(r);
                else
                    x_next(r);
                y_next(r);
                return WALK_BLOCKED;
            }
        } else {
            y_next(r);
            if (solid(r, (uint16_t)r.d[7])) {
                y_prev(r);   /* floor right below: the same height */
            } else {
                y_next(r);
                if (solid(r, (uint16_t)r.d[7]))
                    y_prev(r);   /* floor two below: one down */
            }
        }
    } while (dbra(&r.d[1]));
    return WALK_DONE;
}

/* Coll_WalkBlockMapProbe's work: down one pixel, the floor test (d7 &= the block's mask), back up;
   d7 = 0 when there is no floor there */
static void probe(Regs &r)
{
    y_next(r);
    set_low_word(&r.d[0], word_at(r.a[0]));
    set_low_word(&r.d[7], (uint16_t)(r.d[7] & r.d[0]));
    bool floor = (uint16_t)r.d[7] != 0 && in_column(r);
    y_prev(r);
    if (!floor)
        r.d[7] = 0;
}

/* the walk's end from a loop: blocked, the probe; done, Coll_WalkBlockMapEnd (d6 = 0, the probe) */
static void finish_walk(Regs &r, WalkExit how)
{
    if (how == WALK_DONE)
        r.d[6] = 0;
    probe(r);
}

static Regs load(uint32_t *d0, uint32_t d1, uint32_t d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7,
                 uint32_t a0, uint32_t a3, uint32_t a6)
{
    Regs r = {};
    r.d[0] = *d0, r.d[1] = d1, r.d[3] = d3, r.d[4] = *d4, r.d[5] = *d5, r.d[6] = *d6, r.d[7] = *d7;
    r.a[0] = a0, r.a[3] = a3, r.a[6] = a6;
    return r;
}

/* hands the cursor on to Coll_BlockMapResult (bra.w: the next file's), which writes d0/d3/d4/d5 */
static void to_result(const Regs &r, uint32_t *d0, uint16_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                      uint32_t *d7)
{
    *d0 = r.d[0], *d3 = (uint16_t)r.d[3], *d4 = r.d[4], *d5 = r.d[5], *d6 = r.d[6], *d7 = r.d[7];
    dk::jump(at_Coll_BlockMapResult);
}

/* Coll_WalkBlockMap - terrain move for a grounded mover: walks d1 pixels sideways along the block
 * map, climbing or descending to follow the floor (Coll_WalkBlockMapRight / Left), then probes the
 * pixel below (Coll_WalkBlockMapProbe); with d1 = 0 only the probe.
 * In:  d1 = the x move, d4/d5 = the position, d6/d7 = wall / floor masks
 * Out: d4/d5, d3 as Coll_BlockMapResult (jumped to); d6 = the wall mask hit (0 none), d7 = the floor
 *      mask under the feet (0 = left the ground)
 * Evidence: lea g_LayerA,a6 / movea.l $C(a6),a3 / the cursor set-up as Coll_MoveBlockMap's but
 *           bmi (a 0 index moves the map pointer by 0) / the block load / tst.w d1 / beq.w
 *           Coll_WalkBlockMapEnd / bmi / subq.w #1,d1 / bra.w Coll_WalkBlockMapRight / neg.w /
 *           subq.w #1 / bra.w Coll_WalkBlockMapLeft; called by Coll_MoveOnBlockMap (bit 0 of the
 *           motion flags); 12k TAS frames
 */
void Coll_WalkBlockMap(uint32_t *d0, uint16_t d1, uint16_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                       uint32_t *d7)
{
    Regs r = load(d0, d1, *d3, d4, d5, d6, d7, 0, 0, 0);
    r.a[6] = dk::addr(g_LayerA);
    const MapLayer *L = layer(r);
    r.a[3] = L->map.raw();
    if (cursor_start(r, &r.d[4], (uint16_t)L->width)) {
        set_low_word(&r.d[0], (uint16_t)(r.d[0] * 2));
        r.a[3] += sext((uint16_t)r.d[0]);
    }
    r.d[4] = swap(r.d[4]);
    set_low_word(&r.d[4], (uint16_t)((r.d[4] * 2) & 0x1E));
    if (cursor_start(r, &r.d[5], (uint16_t)L->height)) {
        r.d[0] = (uint32_t)(uint16_t)r.d[0] * (uint16_t)L->map_row_bytes;   /* mulu.w: the whole register */
        r.a[3] += r.d[0];
    }
    r.d[5] = swap(r.d[5]);
    set_low_word(&r.d[5], (uint16_t)(r.d[5] & 0xF));
    load_block(r);

    int16_t dx = (int16_t)d1;
    if (dx == 0) {
        finish_walk(r, WALK_DONE);
    } else if (dx > 0) {
        set_low_word(&r.d[1], (uint16_t)(dx - 1));
        finish_walk(r, walk(r, true));
    } else {
        set_low_word(&r.d[1], (uint16_t)(-dx - 1));
        finish_walk(r, walk(r, false));
    }
    to_result(r, d0, d3, d4, d5, d6, d7);
}

/* Coll_WalkBlockMapEnd - the walk's end: no wall hit (d6 = 0), then the probe below.
 * In:  the cursor (d4/d5/a0/a3, a6 = g_LayerA), d7 = the floor mask
 * Out: d6 = 0; as Coll_WalkBlockMapProbe
 * Evidence: moveq #0,d6 falling into Coll_WalkBlockMapProbe; branched to by Coll_WalkBlockMap (d1 =
 *           0) and by the walk loops when their pixels are used up
 */
void Coll_WalkBlockMapEnd(uint32_t *d0, uint16_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7,
                          uint32_t a0, uint32_t a3, uint32_t a6)
{
    Regs r = load(d0, 0, *d3, d4, d5, d6, d7, a0, a3, a6);
    finish_walk(r, WALK_DONE);
    to_result(r, d0, d3, d4, d5, d6, d7);
}

/* Coll_WalkBlockMapProbe - probes the pixel below the walker (down one, the floor test, back up):
 * d7 = the floor mask there, 0 when there is no floor; then Coll_BlockMapResult.
 * In:  the cursor (d4/d5/a0/a3, a6 = g_LayerA), d7 = the floor mask
 * Out: d7 = the floor under the feet (0 none); d4/d5/d3 as Coll_BlockMapResult
 * Evidence: addq.w #1,d5 ... move.w (a0),d0 / and.w d0,d7 / beq / the column test / back up a pixel
 *           on both paths (moveq #0,d7 on the miss) / bra.w Coll_BlockMapResult; branched to by the
 *           walk loops after a wall
 */
void Coll_WalkBlockMapProbe(uint32_t *d0, uint16_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7,
                            uint32_t a0, uint32_t a3, uint32_t a6)
{
    Regs r = load(d0, 0, *d3, d4, d5, d6, d7, a0, a3, a6);
    probe(r);
    to_result(r, d0, d3, d4, d5, d6, d7);
}

/* Coll_WalkBlockMapRight - the rightward loop of the floor-following walk (also finishes a
 * horizontal move after Coll_MoveBlockMap stopped vertically).
 * In:  d1 = the pixels left - 1; the cursor (d4/d5/a0/a3, a6 = g_LayerA), d6/d7 the masks
 * Out: as Coll_WalkBlockMap
 * Evidence: addq.w #2,d4 ... the wall test (d6) / up a pixel / the wall test again (move.w d0,d6,
 *           back, bra Coll_WalkBlockMapProbe) / the floor tests (d7) one and two pixels down / dbra
 *           d1,Coll_WalkBlockMapRight / bra.w Coll_WalkBlockMapEnd; entered from Coll_WalkBlockMap
 *           (d1 > 0) and Coll_MoveBlockMap (g_CollMoveDX >= 0); 45 corpus runs
 */
void Coll_WalkBlockMapRight(uint32_t *d0, uint16_t d1, uint16_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                            uint32_t *d7, uint32_t a0, uint32_t a3, uint32_t a6)
{
    Regs r = load(d0, d1, *d3, d4, d5, d6, d7, a0, a3, a6);
    finish_walk(r, walk(r, true));
    to_result(r, d0, d3, d4, d5, d6, d7);
}

/* Coll_WalkBlockMapLeft - the leftward loop of the floor-following walk, mirrored.
 * In:  d1 = the pixels left - 1; the cursor (d4/d5/a0/a3, a6 = g_LayerA), d6/d7 the masks
 * Out: as Coll_WalkBlockMap
 * Evidence: subq.w #2,d4 ... as Coll_WalkBlockMapRight with the x steps swapped / dbra
 *           d1,Coll_WalkBlockMapLeft / bra.w Coll_WalkBlockMapEnd; entered from Coll_WalkBlockMap
 *           (d1 < 0) and Coll_MoveBlockMap (g_CollMoveDX < 0); 43 corpus runs
 */
void Coll_WalkBlockMapLeft(uint32_t *d0, uint16_t d1, uint16_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                           uint32_t *d7, uint32_t a0, uint32_t a3, uint32_t a6)
{
    Regs r = load(d0, d1, *d3, d4, d5, d6, d7, a0, a3, a6);
    finish_walk(r, walk(r, false));
    to_result(r, d0, d3, d4, d5, d6, d7);
}
