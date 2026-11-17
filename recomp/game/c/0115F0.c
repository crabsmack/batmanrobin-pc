#include "game.h"

/* HUD_DrawScoreDigit - emits one decimal digit of the score: counts how often the place value goes
 * into it (repeated subtraction), then copies that digit's tile (HUD_CopyScoreDigit).
 * In:  d0 = the value left, d1 = the place value, a0 = the digit tile-index table, a1 = the tiles,
 *      a2 = the buffer position, d7 = the tiles left
 * Out: d0 = the remainder, d2 = the digit x 2 (its low word: moveq #-2 then addq.w, so the upper
 *      word is $FFFF), a2 one column on ($60), d7 - 1 (low word)
 * Evidence: moveq #-2,d2 / addq.w #2,d2 / sub.l d1,d0 / bcc / add.l d1,d0, falling into
 *           HUD_CopyScoreDigit; HUD_DrawScore calls it for 100000 .. 10
 */
void HUD_DrawScoreDigit(uint32_t *d0, uint32_t d1, uint32_t *d2, uint32_t *d7, uint32_t a0, uint32_t a1,
                        uint32_t *a2)
{
    *d2 = 0xFFFFFFFEu;
    bool borrow;
    do {
        *d2 = (*d2 & 0xFFFF0000u) | (uint16_t)(*d2 + 2);
        borrow = *d0 < d1;
        *d0 -= d1;
    } while (!borrow);
    *d0 += d1;
    uint32_t d3 = 0;   /* a clobber of this contract */
    HUD_CopyScoreDigit(*d2, &d3, d7, a0, a1, a2);
}

/* HUD_CopyScoreDigit - copies one digit's tile into the HUD buffer and moves one column on.
 * In:  d2 = the digit x 2 (low word), a0 = the digit tile-index table, a1 = the tiles, a2 = the
 *      buffer position, d7 = the tiles left
 * Out: d3 low word = the tile's offset, a2 + $60, d7 - 1 (low word)
 * Evidence: move.w (a0,d2.w),d3 / lsl.w #5,d3 / lea (a1,d3.w),a3 / 8 x move.l (a3)+,(a2)+ / lea
 *           $40(a2),a2 / subq.w #1,d7; HUD_DrawScore branches here for the units digit
 */
void HUD_CopyScoreDigit(uint32_t d2, uint32_t *d3, uint32_t *d7, uint32_t a0, uint32_t a1, uint32_t *a2)
{
    PTR(const u16) index;
    index.v = a0 + (uint32_t)(int32_t)(int16_t)(uint16_t)d2;
    uint16_t offset = (uint16_t)(*index.get() << 5);
    *d3 = (*d3 & 0xFFFF0000u) | offset;
    PTR(const u8) src;
    src.v = a1 + (uint32_t)(int32_t)(int16_t)offset;
    PTR(u8) dst;
    dst.v = *a2;
    dk::mem_copy(dst.get(), src.get(), 32);
    *a2 += 0x60;
    *d7 = (*d7 & 0xFFFF0000u) | (uint16_t)(*d7 - 1);
}
