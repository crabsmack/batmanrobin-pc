#include "game.h"

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* ToyChain_Update - per-frame chain update: grows it, places and draws the links, refreshes their
 * body boxes and runs their touch attack
 * In:  self = the chain object (a6: +$E the chain block); motion/vm/spr = its blocks (a3/a4/a5);
 *      a0 passed to ToyChain_Grow
 * Out: d0-d7 / a1 as the four steps left them; a2-a6 kept (the movem)
 * Evidence: movem.l a2-a6,-(a7) / movea.w $E(a6),a2 / bsr ToyChain_Grow / bsr
 *           ToyChain_PlaceAndDrawLinks / bsr ToyChain_PlaceLinkBoxes / bsr ToyChain_LinkTouchAttack /
 *           movem.l (a7)+,a2-a6; Op2_CallNativeRel from the chain loops $037A1E/$037BE8/$037E14/
 *           $03806C; TAS f79928-f112311 (6553 frames). PlaceAndDrawLinks leaves a3 past the link
 *           list, which the next two steps get (the movem restores it only at the end); the movem
 *           itself only keeps a2-a6 over the calls: the C keeps them in locals
 */
void ToyChain_Update(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                     uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t *a1, uint32_t a2, uint32_t a3, Vm2Block *vm,
                     Vm2Sprite *spr, Vm2Obj *self)
{
    ToyChainBlock *chain = toy_chain_block(self);
    ToyChain_Grow(d0, a0, a1, chain, vm, self);
    uint16_t r7 = (uint16_t)*d7;
    uint32_t links = a3;
    ToyChain_PlaceAndDrawLinks(d0, d1, d2, d3, d4, d5, d6, &r7, a1, chain, &links, vm, spr, self);
    set_low_word(d7, r7);
    uint16_t r3 = (uint16_t)*d3, r4 = (uint16_t)*d4, r5 = (uint16_t)*d5, r6 = (uint16_t)*d6;
    ToyChain_PlaceLinkBoxes(&r3, &r4, &r5, &r6, &r7, a1, chain, vm, self);
    set_low_word(d3, r3);
    set_low_word(d4, r4);
    set_low_word(d5, r5);
    set_low_word(d6, r6);
    set_low_word(d7, r7);
    ToyChain_LinkTouchAttack(d0, d1, d2, d3, d4, d5, d6, d7, a1, chain, links, dk::addr(vm), spr, dk::addr(self));
}
