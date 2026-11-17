#include "game.h"

/* Layer_Init - initialises a map layer from a level header of 8 self-relative longs (the block
 * map, two the caller takes, block tiles, block table, collision data, animated tiles and their
 * art) and the scene's plane geometry.
 * In:  tile = the tile word added to every block cell; header = the 8 longs; layer
 * Out: layer: x/y/pending 0, cell_add = tile twice, anim_vram, the plane geometry (g_Plane*,
 *      g_ScreenBlocks*; col/row = plane cells - 2), the pointers, block_col/row = width/height - 1,
 *      width, height, map_row_bytes, map_bytes, the animation cleared; tile = anim_vram, d6 = the
 *      tile word, d1 = width x 2, d2 = height x that (mulu); a1/a2 = the header's first two
 *      pointers, a3 = the block map's words, a4 = the animated tiles' art
 * Evidence: clr.w (a6) .. $6(a6) / move.w d0,$8(a6) / move.w d0,$A(a6); movea.l a0,aN / adda.l
 *           (a0)+,aN x 8; move.w (a3)+,d1 / move.w (a3)+,d2 / move.l a3,$C(a6) / subq.w #1,$26(a6) ...
 *           mulu.w d1,d2 / move.w d2,$2C(a6); the scene-script op $7202 at every scene start (TAS
 *           390-90423); Coll_MoveBlockMap reads the fields
 */
void Layer_Init(uint16_t *tile, uint16_t *d1, uint32_t *d2, uint16_t *d6, const s32 *header, uint32_t *a1,
                uint32_t *a2, uint32_t *a3, uint32_t *a4, MapLayer *layer)
{
    /* the header's long n, as the address it points at */
    auto target = [header](int n) { return dk::addr(&header[n]) + (uint32_t)(int32_t)header[n]; };

    layer->x = 0;
    layer->y = 0;
    layer->pending_x = 0;
    layer->pending_y = 0;
    layer->cell_add = ((uint32_t)*tile << 16) | *tile;
    *d6 = *tile;
    *tile = (uint16_t)((*tile & 0x3FFF) << 5);
    layer->anim_vram = *tile;
    layer->plane_row_bytes = g_PlaneRowBytes;
    layer->plane_bytes = g_PlaneBytes;
    layer->screen_blocks_x = (int16_t)g_ScreenBlocksX;
    layer->screen_blocks_y = (int16_t)g_ScreenBlocksY;
    *d1 = g_PlaneCellsX;
    *d2 = (*d2 & 0xFFFF0000u) | g_PlaneCellsY;
    layer->plane_cells_x = (int16_t)*d1;
    layer->plane_cells_y = (int16_t)(uint16_t)*d2;
    layer->col = (uint16_t)(*d1 - 2);
    layer->row = (uint16_t)((uint16_t)*d2 - 2);

    *a1 = target(0);
    *a2 = target(1);
    *a3 = target(2);
    layer->block_tiles.v = target(3);
    layer->block_table.v = target(4);
    layer->collision.v = target(5);
    PTR(const u16) anim;
    anim.v = target(6);
    layer->anim_count = *anim.get();
    layer->anim_list.v = anim.v + 2;
    *a4 = target(7);
    layer->anim_art.v = *a4;

    PTR(const u16) map;                      /* the block map: width, height, then the blocks */
    map.v = *a3;
    uint16_t width = map.get()[0], height = map.get()[1];
    *a3 += 4;
    layer->map.v = *a3;
    layer->block_col = (int16_t)(width - 1);
    layer->block_row = (int16_t)(height - 1);
    layer->width = (int16_t)width;
    layer->height = (int16_t)height;
    *d1 = (uint16_t)(width * 2);
    layer->map_row_bytes = *d1;
    *d2 = (uint32_t)height * *d1;            /* mulu.w: a long */
    layer->map_bytes = (uint16_t)*d2;
    layer->anim_frame = 0;
    layer->anim_timer = 0;
    layer->anim_delay = 0;
}
