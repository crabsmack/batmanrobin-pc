#include "game.h"

/* Mem_Clear - zeroes `size` bytes from `dst`: 32 bytes a loop as longs, then the rest as words,
 * then a last odd byte.
 * In:  dst (a0), size (d7)
 * Out: odd (d6) = size & 1; size (d7) = $FFFF (the word loop's dbra ran out)
 * Evidence: moveq #0,d0 / lsr.w #5,d7 / 8 x move.l d0,(a0)+ under dbra; (d6 & $1F) >> 1 words;
 *           andi.w #1,d6 / move.b; callers Scene_InitCommon ($FF8000, $75F8 bytes) and $3CC82
 */
void Mem_Clear(uint16_t *odd, uint16_t *size, u8 *dst)
{
    uint16_t bytes = *size;
    u32 *l = (u32 *)dst;
    for (unsigned i = 0; i < (unsigned)(bytes >> 5) * 8u; i++)
        *l++ = 0u;
    u16 *w = (u16 *)l;
    for (unsigned i = 0; i < (unsigned)((bytes & 0x1F) >> 1); i++)
        *w++ = 0u;
    *odd = (uint16_t)(bytes & 1);
    if (*odd)
        *(u8 *)w = 0;
    *size = 0xFFFF;
}
