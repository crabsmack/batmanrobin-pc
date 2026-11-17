#include "game.h"

/* Queueing a block column or row of a map layer into the tile queue ($012A84-$012CBD): the
 * entry's count word (-8 x cells: Layer_WriteTiles' jump back), the first VDP write command,
 * then per 2x2-cell block line two longs of cells with the next command word between them; the
 * map and the plane both wrap (the map at its width / height, the plane at its cell size). */

/* the low word of a data register, the upper word kept (a .w move) */
static inline void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* add.w step,v / bpl / add.w size / cmp.w size / blt / sub.w size: one wrap into 0..size-1 */
static int16_t wrap_once(int16_t v, int16_t size)
{
    if (v < 0)
        v = (int16_t)(v + size);
    if (!(v < size))
        v = (int16_t)(v - size);
    return v;
}

/* the same, looping until it is inside (the offset added to the stepped column / row) */
static int16_t wrap_all(int16_t v, int16_t size)
{
    if (v < 0) {
        do
            v = (int16_t)(v + size);
        while (v < 0);
    } else {
        while (!(v < size))
            v = (int16_t)(v - size);
    }
    return v;
}

/* the VDP write command of a plane byte address: (a << 2) split, the address bits low, $4000 set */
static uint32_t vram_write(uint32_t a)
{
    a <<= 2;
    a = (a & 0xFFFF0000u) | ((a & 0xFFFFu) >> 2);
    return ((a << 16) | (a >> 16)) | 0x40000000u;
}

/* the cell loop both share: `lines` block lines from the map at *map (stepping map_step bytes,
   rewinding by rewind bytes when `left` reaches 0), each block's 4 cells as two longs, the command
   word stepping by plane_step (2 cells down a column: twice the row; along a row: 4 bytes),
   wrapped by mask */
static void queue_cells(uint32_t *d0, uint32_t *d1, uint16_t *d2, uint32_t d3, uint16_t mask, uint32_t cells,
                        uint16_t *left, uint16_t *lines, u16 **q, const u8 **map, const u8 *tiles, int16_t map_step,
                        int16_t rewind, uint16_t reload, uint16_t row_step, uint16_t next_step, bool post_inc)
{
    bool first = true;
    for (;;) {
        if (!first)
            *(*q)++ = *d2;
        first = false;
        uint16_t block = *(const u16 *)*map;
        if (post_inc)
            *map += 2;
        const u32 *pair = (const u32 *)(tiles + (int16_t)block);
        *d0 = (uint32_t)pair[0] + cells;
        *d1 = (uint32_t)pair[1] + cells;
        *(*q)++ = (uint16_t)(*d0 >> 16);
        *(*q)++ = (uint16_t)*d0;
        *d2 = (uint16_t)(*d2 + row_step);
        *(*q)++ = *d2;
        *(*q)++ = (uint16_t)(*d1 >> 16);
        *(*q)++ = (uint16_t)*d1;
        *d2 = (uint16_t)(((uint16_t)(*d2 + next_step) & mask) | (uint16_t)d3);
        *map += map_step;
        *left = (uint16_t)(*left - 1);
        if (*left != 0) {                    /* dbeq d7: the map's edge not reached */
            if ((*lines)-- == 0)
                return;
            continue;
        }
        *map -= rewind;                      /* the map wraps: back to its first row / column */
        *left = reload;
        if ((*lines)-- == 0)                 /* dbra d7 */
            return;
    }
}

/* Layer_QueueColumn - steps the layer's block column and queues one column of blocks
 * (g_ScreenBlocksY tall) at an offset from it into the tile queue.
 * In:  step = the block column step, d1 = the column offset to draw, d2 = the plane cell column
 *      step, d3 = the cell column offset; layer
 * Out: layer->block_col, col; the queue entry, g_TileQueuePtr (also a0); a2 = the map after the
 *      last block, a3 = the block tiles; d1 = the last long of cells, d2 = the next command word,
 *      d3 = the command's high bits, d4 = plane_bytes - 1, d5 = cell_add, d6/d7 the counts left
 * Evidence: add.w $26(a6),d0 / wraps / move.w d0,$26(a6); mulu.w $2A(a6) row offset; the plane
 *           column + $24(a6) and mulu.w $36(a6); lsl.l #2 / lsr.w #2 / swap / ori.l #$40000000;
 *           movem.l (a3,d0.w),d0-d1 / add.l d5; subq.w #1,d6 / dbeq d7 / suba.w $2C(a6),a2; callers
 *           Layer_ScrollStepX, Layer_DrawScreen, the scene-script op $7416
 */
void Layer_QueueColumn(uint16_t step, uint32_t *d1, uint16_t *d2, uint32_t *d3, uint16_t *d4, uint32_t *d5,
                       uint16_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a2, uint32_t *a3, uint32_t a6)
{
    PTR(MapLayer) at;
    at.v = a6;
    MapLayer *l = at.get();
    int16_t width = l->width;
    int16_t col = wrap_once((int16_t)(step + l->block_col), width);
    l->block_col = col;
    uint16_t at_col = (uint16_t)wrap_all((int16_t)(col + (int16_t)(uint16_t)*d1), width);
    *d1 = (uint32_t)(uint16_t)l->block_row * l->map_row_bytes;
    uint16_t offset = (uint16_t)(at_col * 2 + (uint16_t)*d1);
    const u8 *map = (const u8 *)l->map.get() + (int16_t)offset;
    *a3 = l->block_tiles.raw();
    int16_t cells_x = l->plane_cells_x;
    int16_t cell = wrap_once((int16_t)(*d2 + l->col), cells_x);
    l->col = (uint16_t)cell;
    cell = wrap_once((int16_t)(cell + (int16_t)(uint16_t)*d3), cells_x);
    *d2 = (uint16_t)((uint16_t)(cell * 2) + l->plane_vram);
    *d3 = (uint32_t)l->row * l->plane_row_bytes;
    set_low_word(d3, (uint16_t)(*d3 + *d2));
    *d3 = vram_write(*d3);
    *d5 = l->cell_add;
    *d7 = (uint16_t)l->screen_blocks_y;
    *d6 = (uint16_t)(l->height - l->block_row);
    PTR(u16) qp;
    qp.v = g_TileQueuePtr;
    u16 *q = qp.get();
    uint32_t d0 = (uint16_t)-(int16_t)(uint16_t)(*d7 << 3);
    *q++ = (uint16_t)d0;
    *q++ = (uint16_t)(*d3 >> 16);
    *q++ = (uint16_t)*d3;
    *d3 = (*d3 << 16) | (*d3 >> 16);
    *d2 = (uint16_t)*d3;
    *d4 = (uint16_t)(l->plane_bytes - 1);
    set_low_word(d3, (uint16_t)(((uint16_t)*d3 & *d4) ^ *d2));   /* the command's bits above the plane */
    *d7 = (uint16_t)(*d7 - 1);
    PTR(const u8) tiles;
    tiles.v = *a3;
    queue_cells(&d0, d1, d2, *d3, *d4, *d5, d6, d7, &q, &map, tiles.get(), (int16_t)l->map_row_bytes,
                (int16_t)l->map_bytes, (uint16_t)l->height, l->plane_row_bytes, l->plane_row_bytes, false);
    *a2 = dk::addr(map);
    *a0 = dk::addr(q);
    g_TileQueuePtr = *a0;
}

/* Layer_ScrollStepY - the y half of Layer_ScrollStep: moves y by the pending y clamped to +-16
 * and, crossing a block edge, queues the block row coming in at the top (or the bottom: cell row
 * offset $1E) across g_ScreenBlocksX columns.
 * In:  layer
 * Out: layer->y, pending_y, block_row, row; on a crossing the queue entry and g_TileQueuePtr (a0),
 *      the registers as Layer_QueueColumn leaves them (d4 = plane_row_bytes - 1 here); d1 = the
 *      clamped step when nothing is queued
 * Evidence: move.w $6(a6),d1 / cmpi.w #$10 / bgt / cmpi.w #$FFF0 / bgt; sub.w d1,$6(a6); not.w d0 /
 *           andi.w #$F / add.w d1,d0 / bcs (up); andi.w #$F / sub.w d1,d0 / bcc (down); moveq -1/0/-2/0
 *           or 1/$F/2/$1E; the row loop with (a2)+ and addq.w #4,d2; reached by bra from
 *           Layer_ScrollStep (75 corpus runs)
 */
void Layer_ScrollStepY(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint32_t *d5, uint16_t *d6,
                       uint16_t *d7, uint32_t *a0, uint32_t *a2, uint32_t *a3, uint32_t a6)
{
    PTR(MapLayer) at;
    at.v = a6;
    MapLayer *l = at.get();
    int16_t dy = l->pending_y;
    set_low_word(d1, (uint16_t)dy);
    if (dy == 0)
        return;
    if (dy > 0x10)
        dy = 0x10;
    else if (dy <= -0x10)                    /* cmpi.w #$FFF0 / bgt: -16 and below become -16 */
        dy = -0x10;
    set_low_word(d1, (uint16_t)dy);
    l->pending_y -= dy;
    uint16_t y = (uint16_t)(l->y + dy);
    l->y = y;
    int16_t row_step, row_offset, cell_step, cell_offset;
    if (dy < 0) {
        uint16_t room = (uint16_t)(~y & 0xF);
        if ((uint32_t)room + (uint16_t)dy > 0xFFFF)   /* add.w carry: no edge crossed */
            return;
        row_step = -1, row_offset = 0, cell_step = -2, cell_offset = 0;
    } else {
        if ((y & 0xF) >= (uint16_t)dy)       /* sub.w no borrow: still inside the block */
            return;
        row_step = 1, row_offset = 0xF, cell_step = 2, cell_offset = 0x1E;
    }
    int16_t height = l->height;
    int16_t row = wrap_once((int16_t)(row_step + l->block_row), height);
    l->block_row = row;
    uint16_t at_row = (uint16_t)wrap_all((int16_t)(row + row_offset), height);
    uint32_t d0 = (uint32_t)at_row * l->map_row_bytes;
    *d1 = (uint16_t)(l->block_col * 2);      /* the moveq before cleared the upper word */
    set_low_word(&d0, (uint16_t)(d0 + (uint16_t)*d1));
    const u8 *map = (const u8 *)l->map.get() + (int16_t)(uint16_t)d0;
    *a3 = l->block_tiles.raw();
    int16_t cells_y = l->plane_cells_y;
    int16_t cell = wrap_once((int16_t)(cell_step + l->row), cells_y);
    l->row = (uint16_t)cell;
    cell = wrap_once((int16_t)(cell + cell_offset), cells_y);
    *d2 = (uint32_t)(uint16_t)cell * l->plane_row_bytes;
    *d3 = (uint16_t)((uint16_t)(l->col * 2) + l->plane_vram);
    set_low_word(d2, (uint16_t)(*d2 + (uint16_t)*d3));
    *d3 = vram_write(*d2);
    *d5 = l->cell_add;
    *d6 = (uint16_t)(l->width - l->block_col);
    *d7 = (uint16_t)l->screen_blocks_x;
    PTR(u16) qp;
    qp.v = g_TileQueuePtr;
    u16 *q = qp.get();
    d0 = (d0 & 0xFFFF0000u) | (uint16_t)-(int16_t)(uint16_t)(*d7 << 3);
    *q++ = (uint16_t)d0;
    *q++ = (uint16_t)(*d3 >> 16);
    *q++ = (uint16_t)*d3;
    *d3 = (*d3 << 16) | (*d3 >> 16);
    uint16_t w2 = (uint16_t)*d3;
    *d4 = (uint16_t)(l->plane_row_bytes - 1);
    set_low_word(d3, (uint16_t)(((uint16_t)*d3 & *d4) ^ w2));   /* the command's bits above the row */
    *d7 = (uint16_t)(*d7 - 1);
    PTR(const u8) tiles;
    tiles.v = *a3;
    queue_cells(&d0, d1, &w2, *d3, *d4, *d5, d6, d7, &q, &map, tiles.get(), 0, (int16_t)l->map_row_bytes,
                (uint16_t)l->width, l->plane_row_bytes, 4, true);
    set_low_word(d2, w2);
    *a2 = dk::addr(map);
    *a0 = dk::addr(q);
    g_TileQueuePtr = *a0;
}

/* Layer_FlushTileQueueIfIdle - V-int list callback: writes the tile queue to the VDP
 * (Layer_FlushTileQueue) unless g_FrameBusy (the main loop is still building the frame).
 * In:  a0 / a1 = the VDP control / data ports (from the V-int); g_FrameBusy
 * Out: as Layer_FlushTileQueue, or nothing
 * Evidence: tst.b (g_FrameBusy).w / beq Layer_FlushTileQueue / rts; registered with `lea
 *           Layer_FlushTileQueueIfIdle,a0 / jsr $8B7E` in 8 stage setups ($01FC98, $032F74 ...); 113
 *           corpus runs
 */
void Layer_FlushTileQueueIfIdle(uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                                uint32_t d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a5, uint32_t *a6)
{
    if (g_FrameBusy)
        return;
    dk::jump(at_Layer_FlushTileQueue);
}
