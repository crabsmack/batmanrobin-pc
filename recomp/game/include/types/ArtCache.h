/* types/ArtCache.h - the sprite-art VRAM cache's descriptors (docs/NOTES.md "Sprite frames and the
 * VRAM art cache"). An art handle (a word in g_ArtHandles, $8000-$8CFF) holds the address of its
 * descriptor, 0 = not resident. */
#pragma once
#include "dk_mem.h"

static constexpr int ART_DESCS = 50;            /* g_ArtDescs */
static constexpr int ART_DESC_TILES = 12;       /* block tiles a descriptor can list */
static constexpr int ART_BLOCK_TILES = 0x10;    /* tiles per VRAM block */
static constexpr int ART_FIRST_BLOCK_TILE = 0x5F;

typedef struct ArtDesc {
    s8  refcount;                  /* +$00: -users; 0 = released (cached, evictable from the release ring) */
    u8  blocks;                    /* +$01: VRAM blocks it holds */
    u16 ring_slot;                 /* +$02: its slot on the release ring g_ArtRelRing, 0 = not queued */
    u16 owner;                     /* +$04: the handle that points here (cleared when it is evicted) */
    u16 tiles[ART_DESC_TILES];     /* +$06: the first tile of each block */
} ArtDesc;
static_assert(offsetof(ArtDesc, ring_slot) == 0x02, "ArtDesc.ring_slot");
static_assert(offsetof(ArtDesc, owner) == 0x04, "ArtDesc.owner");
static_assert(offsetof(ArtDesc, tiles) == 0x06, "ArtDesc.tiles");
static_assert(sizeof(ArtDesc) == 0x1E, "ArtDesc");
