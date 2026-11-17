#include "game.h"

/* the two caption strings (ROM): 'PLAYER\r' and 'SELECT\r' */
static constexpr uint32_t Tbl_014081 = 0x014081u;
static constexpr uint32_t Tbl_01407A = 0x01407Au;

enum {
    SELECT_DX = 0x10E,     /* SELECT's column: right of PLAYER's by this */
    SELECT_Y_SUM = 0x57,   /* SELECT's y = this - PLAYER's y: the two move in opposite directions */
};

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* PlayerSelect_DrawCaption - draws the player-select captions as vertical sprite-letter columns:
 * PLAYER at the object's x/y (left edge, moving up) and SELECT at x + $10E, y = $57 - y (right
 * edge, moving the other way).
 * In:  self = the caption object
 * Out: sprites added; d4/d5 = SELECT's position going in, d6 = 0; the drawing's d0-d5
 * Evidence: movem.l a1-a6 around two Text_DrawSpritesVertical calls with Tbl_014081 / Tbl_01407A,
 *           d6 = 0, d4 = $3E(a6) (+$10E), d5 = $42(a6) (or $57 - it); png x-robin f540
 */
void PlayerSelect_DrawCaption(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                              uint16_t *d6, const Vm2Obj *self)
{
    *d6 = 0;
    set_low_word(d4, (uint16_t)self->x);
    set_low_word(d5, (uint16_t)self->y);
    Text_DrawSpritesVertical(d0, d1, d2, d3, d4, d5, *d6, Tbl_014081);
    *d6 = 0;
    set_low_word(d4, (uint16_t)(self->x + SELECT_DX));
    set_low_word(d5, (uint16_t)(SELECT_Y_SUM - self->y));
    Text_DrawSpritesVertical(d0, d1, d2, d3, d4, d5, *d6, Tbl_01407A);
}
