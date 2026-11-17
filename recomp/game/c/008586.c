#include "game.h"

/* ArtCache_AddBlocks ($008586-$0085AF) and ArtCache_Acquire ($0085B0-$0085F7). */

/* ArtCache_AddBlocks - appends `count` more 16-tile VRAM blocks, continuing from
 * g_ArtNextBlockTile, to the free block ring.
 * In:  count (d0.w; dbra counter)
 * Out: g_ArtBlocksFree/Spare += count (bytes), g_ArtBlockRingWr and g_ArtNextBlockTile advanced;
 *      count = $FFFF, ring = the new ring write pointer (d1.w: only its low byte steps, so the ring
 *      wraps in 256 bytes), tile = the next block's tile (d2.w)
 * Evidence: add.b d0,(g_ArtBlocksFree).w / add.b d0,(g_ArtBlocksSpare).w / the movea.w d1,a0 /
 *           move.w d2,(a0) / addq.b #2,d1 / addi.w #$10,d2 dbra loop; entered from
 *           ArtCache_AddBlocksInline
 */
void ArtCache_AddBlocks(uint16_t *count, uint16_t *ring, uint16_t *tile)
{
    g_ArtBlocksFree = (uint8_t)(g_ArtBlocksFree + (uint8_t)*count);
    g_ArtBlocksSpare = (uint8_t)(g_ArtBlocksSpare + (uint8_t)*count);
    *ring = g_ArtBlockRingWr;
    *tile = g_ArtNextBlockTile;
    for (uint16_t n = *count; n-- != 0;) {
        *dk::handle<u16>((int16_t)*ring) = *tile;
        *ring = (uint16_t)((*ring & 0xFF00u) | (uint8_t)(*ring + 2));   /* addq.b #2,d1 */
        *tile = (uint16_t)(*tile + ART_BLOCK_TILES);
    }
    *count = 0xFFFF;
    g_ArtBlockRingWr = *ring;
    g_ArtNextBlockTile = *tile;
}

/* ArtCache_Acquire - takes a reference to the art behind a handle, allocating `blocks` VRAM blocks
 * (ArtCache_Alloc) when it is not resident.
 * In:  handle = the art handle's address (a0); blocks (d1.b); d2-d7, a1, a3 as ArtCache_Alloc
 *      takes and leaves them
 * Out: ccr = tst.l of the result (moveq; d0 itself is clobbered: no caller reads it):
 *      0 = allocated (list = the descriptor's block list: the caller loads the tiles),
 *      1 = resident and already in use, 2 = revived from the cache (taken off the free counts),
 *      -1 = not enough free blocks, -2 = no free descriptor. An in-use handle with a refcount
 *      >= 1 (never: users count negative) gets its block count byte bumped, not the refcount
 *      (the original's addq.b #1,(a0) after the (a0)+; kept)
 * Evidence: move.w (a0),d0 / bne, sub.b d1,(g_ArtBlocksFree).w / bcs, subq.b
 *           #1,(g_ArtDescsFree).w / bcs, bsr.w ArtCache_Alloc / moveq #0,d0; subq.b #1,(a0)+ /
 *           bcc / bmi; only static caller $11D04 (handle $ED82, 1 block)
 */
void ArtCache_Acquire(uint16_t *blocks, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                      uint32_t *d7, uint32_t handle, uint32_t *a1, uint32_t *list, dk::Flags *ccr)
{
    PTR(u16) h;
    h.v = handle;
    uint16_t desc_addr = *(u16 *)h;
    int32_t result;
    if (desc_addr == 0) {
        uint8_t want = (uint8_t)*blocks;
        if (g_ArtBlocksFree < want) {                /* sub.b borrowed: given back */
            result = -1;
        } else if (g_ArtDescsFree == 0) {            /* subq.b borrowed: clamped, the blocks given back */
            g_ArtDescsFree = 0;
            result = -2;
        } else {
            g_ArtBlocksFree = (uint8_t)(g_ArtBlocksFree - want);
            g_ArtDescsFree = (uint8_t)(g_ArtDescsFree - 1);
            ArtCache_Alloc(blocks, d2, d3, d4, d5, d6, d7, handle, a1, list);
            result = 0;
        }
    } else {
        ArtDesc *desc = dk::handle<ArtDesc>((int16_t)desc_addr);
        int8_t was = desc->refcount;
        desc->refcount = (int8_t)(was - 1);
        if (was == 0) {                              /* released: in use again */
            g_ArtBlocksFree = (uint8_t)(g_ArtBlocksFree - desc->blocks);
            g_ArtDescsFree = (uint8_t)(g_ArtDescsFree - 1);
            result = 2;
        } else {
            if ((int8_t)(was - 1) >= 0)              /* bmi not taken: bumps +1, the block count */
                desc->blocks = (uint8_t)(desc->blocks + 1);
            result = 1;
        }
    }
    *ccr = dk::Flags::test((uint32_t)result, 4);
}
