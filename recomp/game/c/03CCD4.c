#include "game.h"

/* the a4 piece script run for each revealed cell (Op4_Spawn of a piece, Stage3_RevealPlaceTile,
   its flight and Stage3_RevealDrawTile; ROM data after Stage3_SpawnFlyer's script) */
static constexpr uint32_t Scr4_03CE3E = 0x03CE3Eu;

enum {
    REVEAL_SPOTS = 0x24,         /* 36 burst spots: a 9 x 4 grid */
    REVEAL_SPOT_COLUMNS = 9,
    REVEAL_SPOT_MUL = 0x555,     /* spot = (spot * $555 + 1) mod 64 */
    REVEAL_SPOT_MASK = 0x3F,
    REVEAL_SPOT_OFFSET = 0x14,   /* a cell's pixel = cell * 8 + $14 + camera */
    REVEAL_PATCH = 5,            /* a burst's patch: up to 5 x 5 cells */
    REVEAL_MAP_COLUMNS = 0x28,   /* the cell map: 40 x 22 */
    REVEAL_MAP_ROWS = 0x16,
};

/* Stage3_RevealVIntFlush - V-int list object of the reveal: sends the queued plane-B cells (each a
 * VDP command long to the control port and a name-table word to the data port), then empties the
 * queue
 * In:  ctrl/data = the VDP control/data ports (a0/a1, V-int); the reveal's queue (g_SceneVars+$A
 *      bytes at +$C)
 * Out: the queue emptied (queue_bytes = 0)
 * Evidence: move.w (g_SceneVars+$A).w,d7 / beq / lea (g_SceneVars+$C).w,a4 / move.l (a4)+,(a0) /
 *           move.w (a4)+,(a1) / subq.w #6,d7 / bgt / clr.w; made by Stage3_RevealInit (VIntObj_New),
 *           entries from Stage3_RevealPlaceTile; TAS f97888-f101674 (3787 frames)
 */
void Stage3_RevealVIntFlush(uint32_t ctrl, uint32_t data)
{
    ClearingReveal *rv = clearing_reveal();
    int16_t left = (int16_t)rv->queue_bytes;
    if (left == 0)
        return;
    const RevealCellWrite *w = rv->queue;
    bool more;
    do {
        hw::port_write32(ctrl, w->vdp_cmd);
        hw::port_write16(data, w->name);
        w++;
        more = left > 6;   /* subq.w #6 / bgt: the signed compare of the count with 6 */
        left = (int16_t)(left - 6);
    } while (more);
    rv->queue_bytes = 0;
}

/* the movem.w (a7)+ restore: a word saved on the 68000 stack, sign-extended (movem.w into a data
   register writes the whole register) */
static uint32_t saved_word(const s16 *stack, int i) { return (uint32_t)(int32_t)stack[i]; }

/* Stage3_RevealBurst - one reveal burst: shortens the burst interval (timer 0 = timer 1 -= timer
 * 2), takes the next of the 36 spots in the seed's order, throws a spinning toy figure out of it
 * (ToyFigure_SpawnFlyer) and reveals a random-walk patch of up to 25 cells around it (the piece
 * script per cell)
 * In:  a4 = the director's VM block (timers 0-2); d2/d3, a1/a3/a5/a6 pass to the callees
 * Out: the reveal's seed; d4/d5 = the patch's last cell column/row (or the spot's, sign-extended
 *      by the movem restore), d6/d7 = the walk's last step (d7 = 5 at the end), d0 the last
 *      Rand_Next & 1, d1-d3 the last Fx_RunScript's
 * Evidence: move.w $1C(a4),d0 / sub.w $1E(a4) / move.w d0,$1A(a4)/$1C(a4); the seed loop mulu.w
 *           #$555 / addq.w #1 / andi.l #$3F / cmpi.w #$24 / bge; divu.w #9; two Rand_Next; movem.w
 *           d4-d5 around jsr ToyFigure_SpawnFlyer and movem.w d4-d7 around jsr Fx_RunScript;
 *           Op2_CallNativeRelSaved at $03CBEE, $24 times; TAS f97917-f100195. d2/d3 outputs
 *           (measured as inputs only): verify's first TAS call has the generated code's d2
 *           $6C4A0003 against the input $00013680, Fx_RunScript's d2
 */
void Stage3_RevealBurst(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                        uint32_t *d6, uint32_t *d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5,
                        uint32_t a6)
{
    PTR(Vm2Block) vm;
    vm.v = a4;
    uint16_t interval = (uint16_t)(vm->timer[1] - vm->timer[2]);
    vm->timer[0] = interval;
    vm->timer[1] = interval;

    /* the next spot: the order (s * $555 + 1) mod 64, skipping the values past the grid */
    ClearingReveal *rv = clearing_reveal();
    uint32_t spot = rv->seed;
    do {
        spot = (uint32_t)(uint16_t)spot * REVEAL_SPOT_MUL;
        spot = (spot & 0xFFFF0000u) | (uint16_t)(spot + 1);
        spot &= REVEAL_SPOT_MASK;
    } while ((int16_t)spot >= REVEAL_SPOTS);
    rv->seed = (uint16_t)spot;
    uint16_t row = (uint16_t)(spot / REVEAL_SPOT_COLUMNS);
    uint16_t col = (uint16_t)(spot % REVEAL_SPOT_COLUMNS);
    uint32_t x = (*d4 & 0xFFFF0000u) | (uint16_t)(col << 2);
    uint32_t y = (*d5 & 0xFFFF0000u) | (uint16_t)(row << 2);
    uint32_t r = (uint32_t)Rand_Next();
    r = (r & 0xFFFF0000u) | (r & 3);
    x = (x & 0xFFFF0000u) | (uint16_t)(x + r);
    r = (uint32_t)Rand_Next();
    r = (r & 0xFFFF0000u) | (r & 1);
    y = (y & 0xFFFF0000u) | (uint16_t)(y + r);

    /* the flyer from the spot's pixel (movem.w d4-d5 saved around the spawn) */
    dk::push_word((uint16_t)y);
    dk::push_word((uint16_t)x);
    uint16_t px = (uint16_t)(((uint16_t)x << 3) + REVEAL_SPOT_OFFSET + (uint16_t)(g_CameraX >> 16));
    uint16_t py = (uint16_t)(((uint16_t)y << 3) + REVEAL_SPOT_OFFSET + (uint16_t)(g_CameraY >> 16));
    uint16_t w1 = (uint16_t)*d1;
    uint32_t a2 = 0, ra3 = a3, ra4 = a4, ra5 = a5, ra6 = a6;
    ToyFigure_SpawnFlyer(&w1, px, py, &a2, &ra3, &ra4, &ra5, &ra6);
    *d1 = (*d1 & 0xFFFF0000u) | w1;
    PTR(const s16) top;
    top.v = dk::stack_pointer();
    x = saved_word(top, 0);
    y = saved_word(top, 1);
    dk::set_stack_pointer(top.v + 4);

    /* the patch: a random walk across 5 x 5 cells from the spot, a piece per cell */
    uint32_t dx = 0, dy = 0;
    uint32_t r0 = r;
    for (;;) {
        r0 = (uint32_t)Rand_Next();
        r0 = (r0 & 0xFFFF0000u) | (r0 & 1);
        dx = (dx & 0xFFFF0000u) | (uint16_t)(dx + r0);
        if ((int16_t)dx >= REVEAL_PATCH) {
            dx = (dx & 0xFFFF0000u) | (uint16_t)(dx - REVEAL_PATCH);
            dy = (dy & 0xFFFF0000u) | (uint16_t)(dy + 1);
            if ((int16_t)dy >= REVEAL_PATCH)
                break;
        }
        dk::push_word((uint16_t)dy);
        dk::push_word((uint16_t)dx);
        dk::push_word((uint16_t)y);
        dk::push_word((uint16_t)x);
        uint32_t cx = (x & 0xFFFF0000u) | (uint16_t)(x + dx);
        uint32_t cy = (y & 0xFFFF0000u) | (uint16_t)(y + dy);
        uint32_t cdx = dx, cdy = dy;
        Fx_RunScript(&r0, d1, d2, d3, &cx, &cy, &cdx, &cdy, Scr4_03CE3E, a1, a2, ra3, ra4, ra5, ra6);
        top.v = dk::stack_pointer();
        x = saved_word(top, 0);
        y = saved_word(top, 1);
        dx = saved_word(top, 2);
        dy = saved_word(top, 3);
        dk::set_stack_pointer(top.v + 8);
        dx = (dx & 0xFFFF0000u) | (uint16_t)(dx + 1);
    }
    *d0 = r0;
    *d4 = x;
    *d5 = y;
    *d6 = dx;
    *d7 = dy;
}

/* Stage3_RevealNextCell - reveals the next cell not yet taken: scans the cell map on from the
 * cursor (40 columns x 22 rows) and runs the piece script there; d0 = 1 once the whole map is
 * done
 * In:  the reveal's cursor/column/row (g_SceneVars+0/+2/+4); d4/d5 (their upper words stay);
 *      the other registers pass to Fx_RunScript
 * Out: d0 = 0 a piece started / 1 finished; the cursor, column and row moved to the cell taken;
 *      d1-d7 = Fx_RunScript's (finished: d4 = column 0, d5 = the row past the end, the rest
 *      unchanged)
 * Evidence: move.w (g_SceneVars+2).w,d4 / +4,d5 / movea.w (g_SceneVars).w,a0 / addq.w #1,d4 /
 *           cmpi.w #$28 / moveq #0,d4 / addq.w #1,d5 / cmpi.w #$16 / moveq #1,d0 / tst.w (a0)+ /
 *           bne / jsr Fx_RunScript / moveq #0,d0; Op2_CallNativeRelSwitch at $03CC56 (the
 *           director's fill loop); 3 corpus runs. d1/d2/d3/d6/d7 outputs (measured as inputs
 *           only): verify's first TAS call has the generated code's d1 $00002600 against the input
 *           $FFFF0000, Fx_RunScript's d1
 */
void Stage3_RevealNextCell(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                           uint32_t *d6, uint32_t *d7, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t *a4,
                           uint32_t a5, uint32_t *a6)
{
    ClearingReveal *rv = clearing_reveal();
    uint32_t x = (*d4 & 0xFFFF0000u) | (uint16_t)rv->column;
    uint32_t y = (*d5 & 0xFFFF0000u) | (uint16_t)rv->row;
    const u16 *cell = dk::handle<const u16>((int16_t)rv->cursor);
    for (;;) {
        x = (x & 0xFFFF0000u) | (uint16_t)(x + 1);
        if ((int16_t)x >= REVEAL_MAP_COLUMNS) {
            x = 0;
            y = (y & 0xFFFF0000u) | (uint16_t)(y + 1);
            if ((int16_t)y >= REVEAL_MAP_ROWS) {
                *d4 = x;
                *d5 = y;
                *d0 = 1;
                return;
            }
        }
        if (*cell++ == 0)
            break;
    }
    rv->cursor = (uint16_t)dk::addr(cell);
    rv->column = (int16_t)(uint16_t)x;
    rv->row = (int16_t)(uint16_t)y;
    Fx_RunScript(d0, d1, d2, d3, &x, &y, d6, d7, Scr4_03CE3E, a1, a2, a3, *a4, a5, *a6);
    *d4 = x;
    *d5 = y;
    *d0 = 0;
}
