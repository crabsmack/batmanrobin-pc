#include "game.h"

/* the glyph table, 8 bytes per character from $20 (ROM, after Text_DrawSpritesVertical) */
static constexpr uint32_t Tbl_0141B2 = 0x0141B2u;

/* the low word of a data register, the upper word kept (a .w move) */
static inline void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Text_DrawSprites - draws a string as sprites, one per letter (Tbl_0141B2), each letter's art kept
 * loaded through the frame g_TextGlyphFrames holds for its glyph slot. The x advance of each
 * character rides on the 68000's stack across the sprite calls (move.l (a0)+,-(a7)).
 * In:  text = the string (ends at any byte below $20); x/y = the first letter's position; attr =
 *      the sprite attributes
 * Out: x advanced past the string; d0 = the terminator - $20 (a word, upper word 0), the flags of
 *      that subi.w (borrow: N, C, X set); d1-d3 as Spr_ChangeFrame / Spr_AddFront leave them
 * Evidence: moveq #0,d0 / move.b (a2)+,d0 / subi.w #$20,d0 / bcc / rts; lsl.w #3 / lea Tbl_0141B2 /
 *           move.w (a1),d2 / jsr Spr_ChangeFrame / move.w d1,(a1) / beq / jsr Spr_AddFront / move.l
 *           (a7)+,d0 / swap / add.w d0,d4; only caller Text_DrawAtObject; 11 corpus runs
 */
void Text_DrawSprites(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *x, uint16_t y, uint16_t attr,
                      const u8 *text, dk::Flags *ccr)
{
    PTR(const TextGlyph) table;
    table.v = Tbl_0141B2;
    for (;;) {
        uint8_t c = *text++;
        *d0 = (uint16_t)(c - 0x20);
        if (c < 0x20) {
            *ccr = dk::Flags::compare(c, 0x20, 2, true);   /* subi's borrow sets C and X */
            return;
        }
        *d0 = (uint32_t)(c - 0x20) << 3;
        const TextGlyph *g = table.get() + (c - 0x20);
        dk::push_long(((uint32_t)(uint16_t)g->width << 16) | (uint16_t)g->height);
        int16_t slot = g->slot;
        *d0 = (uint16_t)slot;                     /* move.w: the upper word is still 0 */
        if (slot >= 0) {
            u16 *held = (u16 *)(g_TextGlyphFrames + slot);
            set_low_word(d2, *held);
            set_low_word(d1, g->frame);
            uint32_t entry = dk::addr(g + 1), slot_at = dk::addr(held);
            Spr_ChangeFrame(d0, d1, d2, d3, &entry, &slot_at);
            PTR(u16) kept;
            kept.v = slot_at;
            *kept.get() = (uint16_t)*d1;          /* the frame now shown (or the old one) */
            if ((uint16_t)*d1 != 0) {
                uint16_t frame = (uint16_t)*d1;
                dk::Flags spr_ccr{};   /* its &ccr (Spr_* hand their condition codes back) */
                Spr_AddFront(d0, &frame, *x, y, attr, &entry, &spr_ccr);
                set_low_word(d1, frame);
            }
        }
        uint32_t step = dk::pop_long();
        *d0 = (step << 16) | (step >> 16);        /* swap: the width word low */
        *x = (uint16_t)(*x + (uint16_t)*d0);
    }
}
