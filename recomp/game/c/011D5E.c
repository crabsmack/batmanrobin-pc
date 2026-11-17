#include "game.h"

/* Coll_MoveBlockMap and its pixel steps ($011D5E-$0120A1): an airborne move through plane A's
 * 16x16 block collision map (docs/NOTES.md "Terrain collision").
 *
 * While it runs, d4 and d5 are a cursor in the map: the high word the block column / row, the low
 * word the pixel inside the block (x as a byte offset, pixel x 2, into the block's [top, bottom]
 * column pairs; y as the pixel row 0-15). a3 points at the block's word in the map, a0 at the
 * block's collision data (a solidity mask word, then 16 [top, bottom] byte pairs), a6 at
 * g_LayerA. The move is a Bresenham walk: d3 the error, a2 / a1 its steps (3 x and 2 x the major /
 * minor move), a5 / a4 the major / minor axis's step routine (jmp (a5) / jmp (a4)); d1 / d2 count
 * the x / y pixels left (dbra; -1 = that axis is done). A step into a solid pixel steps back and
 * ends its axis, keeping the block's mask (d6 sideways, d7 vertically). */

/* the step routines, as their code addresses (the 68000 jumps through a4 / a5 and keeps them in
 * g_CollStepX / g_CollStepY, so the C keeps the same values) */
enum : uint32_t {
    COLL_STEP_RIGHT = 0x011E4Eu,
    COLL_STEP_LEFT = 0x011EDAu,
    COLL_STEP_DOWN = 0x011F66u,
    COLL_STEP_UP = 0x012004u,
};

/* the registers as the 68000 has them through the move */
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

/* is the cursor's pixel solid for a mask? d0 low word = the block's mask & it (move.w (a0),d0 /
   and.w mask,d0 / beq / cmp.b 2(a0,d4.w),d5 / bcs / cmp.b 3(a0,d4.w),d5 / bgt: the column's top
   unsigned, its bottom signed) */
static bool solid(Regs &r, uint16_t mask)
{
    set_low_word(&r.d[0], (uint16_t)(word_at(r.a[0]) & mask));
    if ((uint16_t)r.d[0] == 0)
        return false;
    uint32_t column = r.a[0] + sext((uint16_t)r.d[4]);
    uint8_t y = (uint8_t)r.d[5];
    if (y < byte_at(column + 2))
        return false;
    if ((int8_t)y > (int8_t)byte_at(column + 3))
        return false;
    return true;
}

/* dbra on a register's low word: true while it hasn't run out (-1) */
static bool dbra(uint32_t *r)
{
    set_low_word(r, (uint16_t)(*r - 1));
    return (uint16_t)*r != 0xFFFFu;
}

/* Coll_BlockMapResult's work: the cursor back to pixels (x: column x 16 + offset / 2; y: row x 16
   + pixel), the block's attribute in d3 */
static void result(Regs &r)
{
    r.d[0] = swap(r.d[4]);
    set_low_word(&r.d[0], (uint16_t)(r.d[0] << 4));
    set_low_word(&r.d[4], (uint16_t)((int16_t)(uint16_t)r.d[4] >> 1));
    set_low_word(&r.d[4], (uint16_t)(r.d[4] + r.d[0]));
    r.d[0] = swap(r.d[5]);
    set_low_word(&r.d[0], (uint16_t)(r.d[0] << 4));
    set_low_word(&r.d[5], (uint16_t)(r.d[5] + r.d[0]));
    set_low_word(&r.d[3], g_CollBlockAttr);
}

/* how a run of steps ended */
enum Exit { EXIT_RESULT, EXIT_WALK_RIGHT, EXIT_WALK_LEFT };

/* Coll_BlockMapStep onwards: the Bresenham loop and the four step routines, until both axes are
   done (Coll_BlockMapResult) or a vertical stop leaves x pixels to walk (Coll_WalkBlockMapRight /
   Left by g_CollMoveDX's sign) */
static Exit run_steps(Regs &r)
{
    for (;;) {
        /* Coll_BlockMapStep: sub.w a1,d3 / bpl: jmp (a5) / add.w a2,d3 / jmp (a4) */
        set_low_word(&r.d[3], (uint16_t)(r.d[3] - r.a[1]));
        uint32_t step;
        if ((int16_t)(uint16_t)r.d[3] >= 0) {
            step = r.a[5];
        } else {
            set_low_word(&r.d[3], (uint16_t)(r.d[3] + r.a[2]));
            step = r.a[4];
        }
        for (bool again = true; again;) {
            again = false;
            switch (step) {
            case COLL_STEP_RIGHT:
            case COLL_STEP_LEFT: {
                bool right = step == COLL_STEP_RIGHT;
                if (right)
                    x_next(r);
                else
                    x_prev(r);
                if (solid(r, (uint16_t)r.d[6])) {   /* a wall: step back, x is done */
                    set_low_word(&r.d[6], (uint16_t)r.d[0]);
                    r.d[1] = 0xFFFFFFFFu;
                    if (right)
                        x_prev(r);
                    else
                        x_next(r);
                } else if (dbra(&r.d[1])) {
                    break;   /* back to Coll_BlockMapStep */
                } else {
                    r.d[6] = 0;
                }
                if ((int16_t)(uint16_t)r.d[2] < 0)
                    return EXIT_RESULT;
                r.a[5] = g_CollStepY;   /* only y left: every step is y's */
                r.a[4] = r.a[5];
                step = r.a[5];
                again = true;
                break;
            }
            case COLL_STEP_DOWN:
            case COLL_STEP_UP: {
                bool down = step == COLL_STEP_DOWN;
                if (down)
                    y_next(r);
                else
                    y_prev(r);
                if (solid(r, (uint16_t)r.d[7])) {   /* floor or ceiling: step back, y is done */
                    set_low_word(&r.d[7], (uint16_t)r.d[0]);
                    r.d[2] = 0xFFFFFFFFu;
                    if (down)
                        y_prev(r);
                    else
                        y_next(r);
                    if ((int16_t)(uint16_t)r.d[1] < 0)
                        return EXIT_RESULT;
                    return (int16_t)(uint16_t)g_CollMoveDX >= 0 ? EXIT_WALK_RIGHT : EXIT_WALK_LEFT;
                }
                if (dbra(&r.d[2]))
                    break;   /* back to Coll_BlockMapStep */
                r.d[7] = 0;
                if ((int16_t)(uint16_t)r.d[1] < 0)
                    return EXIT_RESULT;
                r.a[5] = g_CollStepX;   /* only x left */
                r.a[4] = r.a[5];
                step = r.a[5];
                again = true;
                break;
            }
            default:
                dk::fatal("Coll_BlockMapStep: step routine $%06X", (unsigned)step);
            }
        }
    }
}

static Regs load(uint32_t *d0, uint32_t d1, uint32_t d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                 uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5,
                 uint32_t *a6)
{
    return Regs{ { *d0, d1, d2, *d3, *d4, *d5, *d6, *d7 }, { *a0, *a1, *a2, *a3, *a4, *a5, *a6 } };
}

static void store(const Regs &r, uint32_t *d0, uint16_t *d1, uint16_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                  uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4,
                  uint32_t *a5, uint32_t *a6)
{
    *d0 = r.d[0], *d1 = (uint16_t)r.d[1], *d2 = (uint16_t)r.d[2], *d3 = r.d[3], *d4 = r.d[4], *d5 = r.d[5];
    *d6 = r.d[6], *d7 = r.d[7];
    *a0 = r.a[0], *a1 = r.a[1], *a2 = r.a[2], *a3 = r.a[3], *a4 = r.a[4], *a5 = r.a[5], *a6 = r.a[6];
}

/* the cursor's start on one axis (move.w pos,d0 / swap pos / asr.w #4,d0 / move.w d0,pos / ble /
   cmp.w size,pos / bcs / move.w size,d0 / subq.w #1,d0): pos's high word keeps the pixel, its low
   word = the block index (swapped back by the caller); true when the map pointer moves to the
   block, d0's low word then = the index, clamped to size - 1 (the index itself is not) */
static bool cursor_start(Regs &r, uint32_t *pos, uint16_t size)
{
    set_low_word(&r.d[0], (uint16_t)*pos);
    *pos = swap(*pos);
    set_low_word(&r.d[0], (uint16_t)((int16_t)(uint16_t)r.d[0] >> 4));
    set_low_word(pos, (uint16_t)r.d[0]);
    if ((int16_t)(uint16_t)r.d[0] <= 0)
        return false;
    if (!((uint16_t)r.d[0] < size))
        set_low_word(&r.d[0], (uint16_t)(size - 1));
    return true;
}

/* Coll_MoveBlockMap - terrain move through plane A's block collision map for an airborne mover:
 * steps the point pixel by pixel along the move (Bresenham, x and y), stopping each axis at its
 * first solid pixel (moving up only mask bit 0 counts: one-way platforms are passed from below).
 * A vertical stop with x pixels left finishes in Coll_WalkBlockMapRight / Left.
 * In:  d1/d2 = x/y move in pixels, d4/d5 = the position, d6/d7 = wall / floor solidity masks
 * Out: d4/d5 = the final position, d6 = the mask hit sideways (0 none), d7 = the mask hit
 *      vertically (0 none), d3 = g_CollBlockAttr (Coll_BlockMapResult); no move: d3 = d6 = d7 = 0;
 *      the other registers as the steps leave them (the prototype passes every one: the steps
 *      chain through jmp (a4) / (a5))
 * Evidence: move.w d1,d0 / or.w d2,d0 / bne / moveq #0,d3/d6/d7; move.w d1,(g_CollMoveDX).w / lea
 *           g_LayerA,a6 / movea.l $C(a6),a3 / the cursor set-up (asr.w #4, $2E/$30 clamps, mulu.w
 *           $2A(a6)) / the block load / tst.w d1 bpl / neg.w / lea $11EDA or $11E4E,a5 / tst.w d2 /
 *           lea $12004 (andi.w #1,d7 / d6) or $11F66,a4 / movea.w d1,a2 / movea.w d2,a1 / dbra d1
 *           (clr.w d6) / dbra d2 (clr.w d7) / move.l a5,(g_CollStepX).w / move.l a4,(g_CollStepY).w
 *           / cmpa.w a1,a2 bgt / exg / a2 x 3 / a1 x 2, falling into Coll_BlockMapStep; 19k TAS
 *           frames
 */
void Coll_MoveBlockMap(uint32_t *d0, uint16_t *d1, uint16_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                       uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                       uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    Regs r = load(d0, *d1, *d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
    set_low_word(&r.d[0], (uint16_t)(r.d[1] | r.d[2]));
    if ((uint16_t)r.d[0] == 0) {
        r.d[3] = 0;
        r.d[6] = 0;
        r.d[7] = 0;
        store(r, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
        return;
    }
    g_CollMoveDX = (uint16_t)r.d[1];
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

    if ((int16_t)(uint16_t)r.d[1] < 0) {
        set_low_word(&r.d[1], (uint16_t)-(int16_t)(uint16_t)r.d[1]);
        r.a[5] = COLL_STEP_LEFT;
    } else {
        r.a[5] = COLL_STEP_RIGHT;
    }
    if ((int16_t)(uint16_t)r.d[2] < 0) {
        set_low_word(&r.d[2], (uint16_t)-(int16_t)(uint16_t)r.d[2]);
        r.a[4] = COLL_STEP_UP;
        set_low_word(&r.d[7], (uint16_t)(r.d[7] & 1));   /* moving up only one-way-proof blocks stop it */
        set_low_word(&r.d[6], (uint16_t)(r.d[6] & 1));
    } else {
        r.a[4] = COLL_STEP_DOWN;
    }
    r.a[2] = sext((uint16_t)r.d[1]);
    r.a[1] = sext((uint16_t)r.d[2]);
    if (!dbra(&r.d[1]))
        set_low_word(&r.d[6], 0);
    if (!dbra(&r.d[2]))
        set_low_word(&r.d[7], 0);
    g_CollStepX = r.a[5];
    g_CollStepY = r.a[4];
    if (!((int32_t)r.a[2] > (int32_t)r.a[1])) {   /* cmpa.w a1,a2 / bgt: y is the major axis */
        uint32_t t = r.a[4];
        r.a[4] = r.a[5];
        r.a[5] = t;
        t = r.a[1];
        r.a[1] = r.a[2];
        r.a[2] = t;
    }
    set_low_word(&r.d[3], (uint16_t)r.a[2]);
    r.a[2] += sext((uint16_t)r.a[2]);
    r.a[2] += sext((uint16_t)r.d[3]);
    r.a[1] += sext((uint16_t)r.a[1]);

    Exit how = run_steps(r);
    if (how == EXIT_RESULT)
        result(r);
    store(r, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
    if (how != EXIT_RESULT)
        dk::jump(how == EXIT_WALK_RIGHT ? at_Coll_WalkBlockMapRight : at_Coll_WalkBlockMapLeft);
}

/* Coll_BlockMapStep - one Bresenham step of Coll_MoveBlockMap (the dbra target of its step
 * routines): takes the minor axis's error off d3 and jumps to the major (a5) or minor (a4) axis's
 * step, which loops back here until both moves are used up.
 * In:  the move's registers (d1/d2 counts, d3 error, a1/a2 its steps, a4/a5 the step routines, the
 *      cursor d4/d5/a0/a3, a6 = g_LayerA, d6/d7 the masks)
 * Out: as Coll_MoveBlockMap
 * Evidence: sub.w a1,d3 / bpl / add.w a2,d3 / jmp (a4) / jmp (a5); dbra d1/d2,Coll_BlockMapStep in
 *           $11E4E, $11EDA, $11F66, $12004
 */
void Coll_BlockMapStep(uint32_t *d0, uint16_t *d1, uint16_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                       uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                       uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    Regs r = load(d0, *d1, *d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
    Exit how = run_steps(r);
    if (how == EXIT_RESULT)
        result(r);
    store(r, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
    if (how != EXIT_RESULT)
        dk::jump(how == EXIT_WALK_RIGHT ? at_Coll_WalkBlockMapRight : at_Coll_WalkBlockMapLeft);
}

/* Coll_BlockMapResult - the block-map moves' common exit: the cursor back to pixels and the
 * block's attribute.
 * In:  d4/d5 = the cursor (block index : offset)
 * Out: d4/d5 low words = the pixel position (the high words keep the block index), d3 low word =
 *      g_CollBlockAttr, d0 = d5 swapped with its low word x 16
 * Evidence: move.l d4,d0 / swap / asl.w #4 / asr.w #1,d4 / add.w d0,d4 / move.l d5,d0 / swap /
 *           asl.w #4 / add.w d0,d5 / move.w (g_CollBlockAttr).w,d3; branched to from
 *           Coll_MoveBlockMap's steps and Coll_WalkBlockMap's (9 references)
 */
void Coll_BlockMapResult(uint32_t *d0, uint16_t *d3, uint32_t *d4, uint32_t *d5)
{
    Regs r = {};
    r.d[0] = *d0;
    r.d[3] = *d3;
    r.d[4] = *d4;
    r.d[5] = *d5;
    result(r);
    *d0 = r.d[0];
    *d3 = (uint16_t)r.d[3];
    *d4 = r.d[4];
    *d5 = r.d[5];
}
