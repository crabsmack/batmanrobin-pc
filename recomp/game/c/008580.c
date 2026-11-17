#include "game.h"

/* ArtCache_AddBlocksInline ($008580-$008585). */

/* ArtCache_AddBlocksInline - adds blocks to the sprite-art VRAM cache, the count from the a6
 * script's next word; the rest is ArtCache_AddBlocks (the next function file).
 * In:  script = the count word (a6)
 * Out: count = the word read, then as ArtCache_AddBlocks leaves count/ring/tile (d0/d1/d2);
 *      script past the word
 * Evidence: move.w (a6)+,d0 / bra.s ArtCache_AddBlocks; reached through a pointer (an a6 native,
 *           TAS frame 4539)
 */
void ArtCache_AddBlocksInline(uint16_t *count, uint16_t *ring, uint16_t *tile, uint32_t *script)
{
    (void)ring; (void)tile;
    PTR(const u16) p;
    p.v = *script;
    const u16 *word = p;
    *count = *word++;
    *script = ADDR68(word);
    dk::jump(at_ArtCache_AddBlocks);
}
