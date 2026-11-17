/* types/Spr.h - the sprite attribute table as the game builds it in RAM (g_SATBuffer, written at
 * g_SATPtr; docs/NOTES.md "Sprites"). */
#pragma once
#include "dk_mem.h"

/* One VDP sprite attribute entry (8 bytes, the VDP's own layout). */
typedef struct SprSatEntry {
    u16 y;                 /* +0: screen y + $80 */
    u8  size;              /* +2: bits 3-2 width - 1, bits 1-0 height - 1 (cells) */
    u8  link;              /* +3: the next entry's index */
    u16 attr;              /* +4: priority, palette, flips, tile */
    u16 x;                 /* +6: screen x + $80 */
} SprSatEntry;
static_assert(sizeof(SprSatEntry) == 8, "SprSatEntry");
static_assert(offsetof(SprSatEntry, attr) == 4, "SprSatEntry.attr");

static constexpr int SPR_MAX_SPRITES = 80;           /* the VDP's sprite limit in H40 */
static constexpr uint16_t SPR_ATTR_HFLIP = 0x0800;   /* the attribute word's flips */
static constexpr uint16_t SPR_ATTR_VFLIP = 0x1000;

/* One piece of a sprite frame (12 bytes; a piece list is a word count - 1, then the pieces). The
 * flipped offsets are stored beside the plain ones: an H/V flip just swaps the long's halves. */
typedef struct SprPiece {
    s16 y_vflip;                   /* +$00: y offset when V-flipped */
    s16 y;                         /* +$02: y offset */
    s16 x_hflip;                   /* +$04: x offset when H-flipped */
    s16 x;                         /* +$06: x offset */
    u16 size;                      /* +$08: the SAT size byte in the high byte (or'd with the link) */
    u8  tile_slot;                 /* +$0A: byte offset into the art descriptor's block tiles */
    u8  tile_add;                  /* +$0B: tiles into that block (replaces the attribute's low byte) */
} SprPiece;
static_assert(sizeof(SprPiece) == 12, "SprPiece");

/* A sprite frame-table entry (10 bytes): Tbl_1DDB60 in ROM for frames >= 0, the RAM table below
 * g_ArtRelRingWr ($A764) for frames < 0 (Spr_InitBankFrames). */
typedef struct SprFrame {
    u16 art_handle;                /* +$00: the art handle's address (in g_ArtHandles) */
    u32 pieces;                    /* +$02: the piece list (word count - 1, then SprPiece) */
    u32 art;                       /* +$06: the art block (word DMA cost, word chunks, chunk codes, tiles) */
} SprFrame;
static_assert(sizeof(SprFrame) == 10, "SprFrame");
static_assert(offsetof(SprFrame, art) == 6, "SprFrame.art");

/* the frame tables: a frame number is a byte offset from one of these */
static constexpr uint32_t SPR_ROM_FRAMES = 0x1DDB60;  /* Tbl_1DDB60, frames >= 0 */
