#include "game.h"

/* the glyph table, 8 bytes per character from $20 (ROM, after this function) */
static constexpr uint32_t Tbl_0141B2 = 0x0141B2u;

/* the low word of a data register, the upper word kept (a .w move) */
static inline void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Text_DrawSpritesVertical - draws a string as a column of sprite letters: each letter centred on
 * the column's x (x - width / 2), the next one lower by the glyph's height. The column x and the
 * next y ride on the 68000's stack across the sprite calls (movem.w d0-d1,-(a7)).
 * In:  string = the string's address (any byte below $20 ends it); x/y = the column's centre x and top y (low
 *      words); attr = the sprite attributes
 * Out: d0 = the terminator - $20 (upper word 0); x/y = the column x and the y below the last
 *      letter, sign-extended (movem.w); d1-d3 as Spr_ChangeFrame / Spr_AddFront leave them
 * Evidence: add.w d4,d4 / sub.w (a0)+,d4 / asr.w #1,d4 / add.w (a0)+,d1 / movem.w d0-d1,-(a7) ...
 *           movem.w (a7)+,d4-d5; only caller PlayerSelect_DrawCaption $49C88 ('PLAYER', 'SELECT'
 *           down the screen edges of the player-select screen, TAS f431-f477)
 */
void Text_DrawSpritesVertical(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *x, uint32_t *y,
                              uint16_t attr, uint32_t string)
{
    PTR(const TextGlyph) table;
    table.v = Tbl_0141B2;
    PTR(const u8) at;                              /* a2 stays a ROM address for the caller's C */
    at.v = string;
    const u8 *text = at.get();
    for (;;) {
        uint8_t c = *text++;
        *d0 = (uint16_t)(c - 0x20);
        if (c < 0x20)
            return;
        const TextGlyph *g = table.get() + (c - 0x20);
        uint16_t column = (uint16_t)*x;
        *d0 = column;                              /* move.w d4,d0: the upper word is still 0 */
        set_low_word(d1, (uint16_t)*y);
        set_low_word(x, (uint16_t)((int16_t)(uint16_t)(column * 2 - g->width) >> 1));
        set_low_word(d1, (uint16_t)(*d1 + g->height));
        dk::push_word((uint16_t)*d1);              /* movem.w d0-d1,-(a7): d0 at the lower address */
        dk::push_word(column);
        int16_t slot = g->slot;
        set_low_word(d0, (uint16_t)slot);
        if (slot >= 0) {
            u16 *held = (u16 *)(g_TextGlyphFrames + slot);
            set_low_word(d2, *held);
            set_low_word(d1, g->frame);
            uint32_t entry = dk::addr(g + 1), slot_at = dk::addr(held);
            Spr_ChangeFrame(d0, d1, d2, d3, &entry, &slot_at);
            PTR(u16) kept;
            kept.v = slot_at;
            *kept.get() = (uint16_t)*d1;
            if ((uint16_t)*d1 != 0) {
                uint16_t frame = (uint16_t)*d1;
                dk::Flags spr_ccr{};   /* its &ccr (Spr_* hand their condition codes back) */
                Spr_AddFront(d0, &frame, (uint16_t)*x, (uint16_t)*y, attr, &entry, &spr_ccr);
                set_low_word(d1, frame);
            }
        }
        uint32_t saved = dk::pop_long();           /* movem.w (a7)+,d4-d5: each word sign-extended */
        *x = (uint32_t)(int32_t)(int16_t)(saved >> 16);
        *y = (uint32_t)(int32_t)(int16_t)saved;
    }
}
