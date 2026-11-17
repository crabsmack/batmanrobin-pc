#include "game.h"

/* the low word of a data register, the upper word kept (a .w operation) */
static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Layer_ScrollStepX - x half of Layer_ScrollStep: moves the layer's x by its pending x scroll,
 * at most 16 px a frame, and when that crosses a block edge queues the block column coming into
 * view (Layer_QueueColumn: d0 = -1 at the left edge, or +1 with column/cell offsets $15/$2A at the
 * right edge).
 * In:  layer = the map layer (a6)
 * Out: layer->x moved, layer->pending_x reduced by the step; no block edge crossed: d0's low word
 *      = the edge test's leftover, d1's = the step (upper words kept); crossed: d0-d3 = the
 *      column's arguments, then Layer_QueueColumn
 * Evidence: move.w $4(a6),d1 / beq / clamp to +-$10 / sub.w d1,$4(a6) / add.w d1,(a6) copy / tst.w
 *           d1 / bpl: andi.w #$F / sub.w d1,d0 / bcc; else not.w d0 / andi.w #$F / add.w d1,d0 / bcs;
 *           moveq -1/0/-2/0 + bra.s Layer_QueueColumn, or moveq 1/$15/2/$2A falling into it
 */
void Layer_ScrollStepX(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint32_t *d5,
                       uint16_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a2, uint32_t *a3, MapLayer *layer)
{
    int16_t step = layer->pending_x;
    set_low_word(d1, (uint16_t)step);
    if (step == 0)
        return;
    if (step > 0x10)
        step = 0x10;
    else if (step <= -0x10)                  /* cmpi.w #$FFF0 / bgt: -16 and below become -16 */
        step = -0x10;
    set_low_word(d1, (uint16_t)step);
    layer->pending_x -= step;
    uint16_t x = (uint16_t)(layer->x + step);
    set_low_word(d0, x);
    layer->x = x;
    if (step < 0) {
        /* moving left: a new block when the step reaches past the offset inside the block */
        uint16_t room = (uint16_t)(~x & 0xF);
        set_low_word(d0, (uint16_t)(room + (uint16_t)step));
        if ((uint32_t)room + (uint16_t)step > 0xFFFF)   /* add.w carry: no edge crossed */
            return;
        *d0 = (uint32_t)-1;
        *d1 = 0;
        *d2 = (uint32_t)-2;
        *d3 = 0;
    } else {
        uint16_t offset = (uint16_t)(x & 0xF);
        set_low_word(d0, (uint16_t)(offset - (uint16_t)step));
        if (offset >= (uint16_t)step)        /* sub.w no borrow: still inside the block */
            return;
        *d0 = 1;
        *d1 = 0x15;
        *d2 = 2;
        *d3 = 0x2A;
    }
    dk::jump(at_Layer_QueueColumn);
}
