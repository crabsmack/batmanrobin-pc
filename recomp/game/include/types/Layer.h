/* types/Layer.h - the map layers' records (docs/NOTES.md "Map layers"). */
#pragma once
#include "dk_mem.h"

/* One write of the tile queue g_TileQueue: a VDP command low word (the cell address) and the
   long of two tile words that goes to the data port. An entry of the queue is a word -(4 x cells)
   (the jump back into the unrolled Layer_WriteTiles; 0 ends the queue), the command's high word,
   then that many of these. */
typedef struct TileQueueCell {
    u16 command;           /* +0: to the control port */
    u32 tiles;             /* +2: to the data port */
} TileQueueCell;
static_assert(sizeof(TileQueueCell) == 6, "TileQueueCell");
static_assert(offsetof(TileQueueCell, tiles) == 2, "TileQueueCell.tiles");

/* A map layer's scroll state (g_LayerA; a6 of the Layer_ functions): the fields the C uses. */
typedef struct MapLayer {
    u16 x;                 /* +0: x position in pixels (its low 4 bits: the offset inside a block) */
    u16 y;                 /* +2: y position */
    s16 pending_x;         /* +4: x scroll still to apply (the camera code adds to it) */
    s16 pending_y;         /* +6: y scroll still to apply */
    u32 cell_add;          /* +$08: the tile word twice (+8/+$A, Layer_Init's d0): added to each 2-cell long */
    PTR(const u16) map;    /* +$0C: the block map's block words (after its width / height words) */
    PTR(const u32) block_tiles;   /* +$10: 2 longs (4 cells) per block, indexed by the block word */
    PTR(const void) block_table;  /* +$14: the level's block table (Coll_MoveBlockMap) */
    PTR(const void) collision;    /* +$18: the collision data */
    PTR(const u16) anim_list;     /* +$1C: animated tile frames, 3 words each: art offset, VRAM offset, bytes */
    PTR(const u8) anim_art;       /* +$20: their art */
    u16 plane_vram;        /* +$24: the plane's name table VRAM address (the scene op $71CC) */
    s16 block_col;         /* +$26: the block column at the screen's left edge (0..width-1) */
    s16 block_row;         /* +$28: the block row at its top (0..height-1) */
    u16 map_row_bytes;     /* +$2A: bytes per block-map row (width x 2) */
    u16 map_bytes;         /* +$2C: the block map's size */
    s16 width;             /* +$2E: the map's width in blocks */
    s16 height;            /* +$30: its height */
    u16 col;               /* +$32: tile column of the plane's left edge (Scroll_LayerToScroll) */
    u16 row;               /* +$34: tile row of its top edge */
    u16 plane_row_bytes;   /* +$36: g_PlaneRowBytes */
    u16 plane_bytes;       /* +$38: g_PlaneBytes */
    s16 plane_cells_x;     /* +$3A: g_PlaneCellsX */
    s16 plane_cells_y;     /* +$3C: g_PlaneCellsY */
    s16 screen_blocks_x;   /* +$3E: g_ScreenBlocksX: block columns a screen shows */
    s16 screen_blocks_y;   /* +$40: g_ScreenBlocksY */
    u16 anim_count;        /* +$42: animated tile frames (0 = none) */
    u16 anim_frame;        /* +$44: the current one */
    s16 anim_timer;        /* +$46: frames until the next */
    u16 anim_delay;        /* +$48: the timer's reload */
    u16 anim_vram;         /* +$4A: VRAM byte address of the animated tiles (tile index x 32) */
} MapLayer;
static_assert(offsetof(MapLayer, cell_add) == 0x08, "MapLayer.cell_add");
static_assert(offsetof(MapLayer, map) == 0x0C, "MapLayer.map");
static_assert(offsetof(MapLayer, anim_list) == 0x1C, "MapLayer.anim_list");
static_assert(offsetof(MapLayer, plane_vram) == 0x24, "MapLayer.plane_vram");
static_assert(offsetof(MapLayer, block_col) == 0x26, "MapLayer.block_col");
static_assert(offsetof(MapLayer, width) == 0x2E, "MapLayer.width");
static_assert(offsetof(MapLayer, plane_row_bytes) == 0x36, "MapLayer.plane_row_bytes");
static_assert(offsetof(MapLayer, screen_blocks_x) == 0x3E, "MapLayer.screen_blocks_x");
static_assert(offsetof(MapLayer, anim_count) == 0x42, "MapLayer.anim_count");
static_assert(offsetof(MapLayer, anim_vram) == 0x4A, "MapLayer.anim_vram");
static_assert(offsetof(MapLayer, pending_x) == 4, "MapLayer.pending_x");
static_assert(offsetof(MapLayer, pending_y) == 6, "MapLayer.pending_y");
static_assert(offsetof(MapLayer, col) == 0x32, "MapLayer.col");
static_assert(offsetof(MapLayer, row) == 0x34, "MapLayer.row");
