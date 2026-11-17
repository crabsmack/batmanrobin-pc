#include "game.h"

/* HUD_DrawPanelText - draws a 12-character panel message as 2 rows of 6 font tiles (each
 * character ASCII - $20 through the glyph table); a row's tiles go every $40 bytes, the second
 * row $20 after the first.
 * In:  a3 = the text, a0 = the glyph tile-index table (Tbl_0586F2), a1 = the font tiles
 *      (Art_058AA2), a2 = the buffer (g_HUDTileBuf)
 * Out: a3 past the 12 characters, a2 = the buffer + $40, a4 past the last tile copied, d6 = d7 =
 *      $FFFF (moveq, then dbra)
 * Evidence: moveq #1,d6 / moveq #5,d7 / moveq #0,d0 / move.b (a3)+,d0 / subi.b #$20,d0 / add.w
 *           d0,d0 / move.w (a0,d0.w),d0 / lsl.w #5,d0 / lea (a1,d0.w),a4 / 8 x move.l / lea
 *           $20(a2),a2 / dbra d7 / lea -$160(a2),a2 / dbra d6; called by HUD_StepP1Panel /
 *           HUD_StepP2Panel in text mode with +$1A set
 */
void HUD_DrawPanelText(uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t a1, uint32_t *a2, uint32_t *a3,
                       uint32_t *a4)
{
    for (int row = 0; row < 2; row++) {
        for (int ch = 0; ch < 6; ch++) {
            PTR(const u8) text;
            text.v = *a3;
            uint8_t glyph = (uint8_t)(*text.get() - 0x20);
            *a3 += 1;
            PTR(const u16) index;
            index.v = a0 + glyph * 2u;
            uint16_t offset = (uint16_t)(*index.get() << 5);
            *a4 = a1 + (uint32_t)(int32_t)(int16_t)offset;
            PTR(const u8) src;
            src.v = *a4;
            PTR(u8) dst;
            dst.v = *a2;
            dk::mem_copy(dst.get(), src.get(), 32);
            *a4 += 32;
            *a2 += 32 + 0x20;
        }
        *a2 -= 0x160;
    }
    *d6 = 0xFFFFu;
    *d7 = 0xFFFFu;
}
