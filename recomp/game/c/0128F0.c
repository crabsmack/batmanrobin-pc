#include "game.h"

/* the low word of a data register, the upper word kept (a .w move) */
static inline void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Layer_MoveY - moves a layer's y by some pixels without drawing: its block row steps by the
 * block edges crossed, wrapping at the map's height.
 * In:  dy = signed pixels; layer
 * Out: layer->y, block_row; dy = its low 4 bits (of -dy going up), d1 = the map's height, d2 = the
 *      new block row
 * Evidence: as Layer_MoveX on +2, +$28 and +$30; the scene-script ops $739E/$73DE and $032F14 (dy =
 *           $1910 for the vertical zeppelin stage, TAS f22200)
 */
void Layer_MoveY(uint16_t *dy, uint16_t *d1, uint16_t *d2, MapLayer *layer)
{
    uint16_t y = (uint16_t)(layer->y + *dy);
    layer->y = y;
    *d2 = *dy;
    if (!(*dy & 0x8000u)) {
        *d2 = (uint16_t)(*d2 >> 4);
        *dy &= 0xF;
        if ((y & 0xF) < *dy)                 /* the offset wrapped past a block edge */
            *d2 = (uint16_t)(*d2 + 1);
    } else {
        *d2 = (uint16_t)-(int16_t)(uint16_t)((uint16_t)-(int16_t)*d2 >> 4);
        *dy = (uint16_t)(-(int16_t)*dy & 0xF);
        if ((~y & 0xF) < *dy)
            *d2 = (uint16_t)(*d2 - 1);
    }
    *d1 = (uint16_t)layer->height;
    *d2 = (uint16_t)(*d2 + layer->block_row);
    if ((int16_t)*d2 < 0)
        *d2 = (uint16_t)(*d2 + *d1);
    if (!((int16_t)*d2 < (int16_t)*d1))
        *d2 = (uint16_t)(*d2 - *d1);
    layer->block_row = (int16_t)*d2;
}

/* Layer_DrawScreen - draws a whole screen of the layer straight to the VDP: g_ScreenBlocksX block
 * columns, each queued by Layer_QueueColumn and written at once by Layer_FlushTileQueue; the
 * block and cell columns come back as they were. (The original saves them and its column count
 * on the 68000's stack; nothing in between reads it, so here they are locals.)
 * In:  layer
 * Out: the plane's cells; d2-d6 as the last column's Layer_QueueColumn leaves them (d2 moveq #2 and
 *      d3 #0 going in); a3 / a4 Layer_FlushTileQueue's
 * Evidence: move.w $26(a6),-(a7) / move.w $32(a6),-(a7) / move.w $3E(a6),d7 / subq.w #1,d7; moveq
 *           #1,d0 / #0,d1 / #2,d2 / #0,d3 / bsr Layer_QueueColumn / lea ($C00004).l,a0 / lea -4(a0),a1 /
 *           bsr Layer_FlushTileQueue / dbra; 8 scene setups ($00740E, $01FC92, $032F6E ...)
 */
void Layer_DrawScreen(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t d7, uint32_t *a3,
                      uint32_t *a4, uint32_t a5, uint32_t *layer)
{
    PTR(MapLayer) at;
    at.v = *layer;
    MapLayer *l = at.get();
    int16_t block_col = l->block_col;
    uint16_t col = l->col;
    uint16_t n = (uint16_t)(l->screen_blocks_x - 1);
    uint32_t a0 = 0, a2 = 0;
    for (;;) {
        uint32_t d1 = 0;
        *d2 = 2;
        *d3 = 0;
        uint16_t w2 = (uint16_t)*d2, w4 = (uint16_t)*d4, w6 = (uint16_t)*d6, w7 = n;
        Layer_QueueColumn(1, &d1, &w2, d3, &w4, d5, &w6, &w7, &a0, &a2, a3, *layer);
        set_low_word(d2, w2);
        set_low_word(d4, w4);
        set_low_word(d6, w6);
        uint16_t displacement = 0;
        u16 *queue = nullptr, *old_end = nullptr;
        Layer_FlushTileQueue(&displacement, hw::VDP_CTRL, hw::VDP_DATA, &queue, &old_end);
        *a3 = dk::addr(queue);
        if (old_end)                         /* an empty queue leaves a4 as it was */
            *a4 = dk::addr(old_end);
        if (n-- == 0)                        /* dbra d7 */
            break;
    }
    l->col = col;
    l->block_col = block_col;
}
