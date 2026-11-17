#include "game.h"

/* ArtCache_Init ($008514-$00857F): the sprite-art VRAM cache at scene init. */

/* ArtCache_Init - sets up the sprite-art VRAM cache: `blocks` 16-tile blocks from tile $5F on the
 * free block ring, the 50 descriptors on the free-descriptor ring, an empty release ring and every
 * art handle cleared.
 * In:  blocks (d1.w: the scene header's art block count)
 * Out: g_ArtHandles ($8000-$8CFF) cleared; g_ArtBlockRing = tiles $5F, $6F, ..., g_ArtDescRing =
 *      the g_ArtDescs addresses; the ring pointers, g_ArtBlocksFree/Spare = blocks,
 *      g_ArtDescsFree/Spare = 50, g_ArtNextBlockTile = the tile after the last block; blocks and
 *      descs = $FFFF (the dbra counters: d1, d2's low word), desc_end = the descriptor ring's
 *      write end (a2), rel_ring = g_ArtRelRing (a3)
 * Evidence: lea (g_ArtHandles).w,a0 / ... / move.w #$680,d0 / moveq #$32,d2, the clr.w (a0)+
 *           dbra, move.w #$5F,d0 / addi.w #$10,d0 per block, move.w a0,(a2)+ / lea $1E(a0),a0
 *           per descriptor; only caller the scene init $9AAC
 */
void ArtCache_Init(uint16_t *blocks, uint32_t *descs, uint32_t *desc_end, uint32_t *rel_ring)
{
    if (*blocks == 0)   /* subq.w #1 / dbra would fill 65,536 ring words: over all of RAM */
        dk::fatal("ArtCache_Init: 0 blocks");
    uint8_t n = (uint8_t)*blocks;
    g_ArtBlocksFree = n;
    g_ArtBlocksSpare = n;
    g_ArtDescsFree = ART_DESCS;
    g_ArtDescsSpare = ART_DESCS;
    g_ArtRelRingRd = dk::handle_of(g_ArtRelRing);
    g_ArtRelRingWr = dk::handle_of(g_ArtRelRing);
    dk::mem_set(&g_ArtHandles, 0, 0x680 * 2);

    u16 *ring = (u16 *)g_ArtBlockRing;
    g_ArtBlockRingRd = dk::handle_of(ring);
    uint16_t tile = ART_FIRST_BLOCK_TILE;
    for (uint16_t i = 0; i < *blocks; i++, tile += ART_BLOCK_TILES)
        *ring++ = tile;
    g_ArtBlockRingWr = dk::handle_of(ring);
    g_ArtNextBlockTile = tile;

    u16 *slot = (u16 *)g_ArtDescRing;
    g_ArtDescRingRd = dk::handle_of(slot);
    ArtDesc *desc = (ArtDesc *)g_ArtDescs;
    for (int i = 0; i < ART_DESCS; i++)
        *slot++ = dk::handle_of(desc++);
    g_ArtDescRingWr = dk::handle_of(slot);

    *blocks = 0xFFFF;
    *descs = 0x0000FFFFu;
    *desc_end = ADDR68(slot);
    *rel_ring = ADDR68(g_ArtRelRing);
}
