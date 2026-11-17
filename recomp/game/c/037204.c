#include "game.h"

enum {
    CHAIN_LINK_SPACING = 0x14,     /* a link every 20 px of extension */
};

/* the extension's integer part (subi.w to the long's high word) */
static void shorten(ToyChainBlock *chain)
{
    uint32_t e = (uint32_t)chain->extension;
    chain->extension = (int32_t)((uint32_t)(uint16_t)((e >> 16) - CHAIN_LINK_SPACING) << 16 | (e & 0xFFFFu));
}

/* ToyChain_Grow - grows a chain: the extension advances by the grow speed and a link is added each
 * time it reaches 0 (then 20 px back), first for the initial links, then at the second speed back
 * up to the maximum (regrowing links lost to hits); a grow speed of 0 adds the initial links one a
 * frame
 * In:  chain = the chain block (a2); vm = the chain's VM block (a4: +$24 the link count); self =
 *      the chain object (a6); a0 passed to ToyChain_AddLink
 * Out: the chain block's extension / grow speed / links to grow; links added (the extension
 *      cleared when the pool is full or the chain is complete); d0 / a1 as the last step left
 *      them
 * Evidence: move.l $10(a2),d0 / beq / add.l d0,$C(a2) / bmi / tst.w $2A(a2) / beq / bsr
 *           ToyChain_AddLink / bne / subi.w #$14,$C(a2) / subq.w #1,$2A(a2) / bne / move.l
 *           $14(a2),$10(a2); move.w $24(a4),d0 / cmp.w $2C(a2),d0 / blt; clr.l $C(a2); called by
 *           ToyChain_Update; TAS f79928-f112311
 */
void ToyChain_Grow(uint32_t *d0, uint32_t a0, uint32_t *a1, ToyChainBlock *chain, Vm2Block *vm, Vm2Obj *self)
{
    *d0 = (uint32_t)chain->grow_speed;
    if (*d0 == 0) {
        /* no growth speed: one initial link a frame */
        if (chain->grow_left == 0)
            return;
        ToyChain_AddLink(d0, a0, a1, chain, vm, self);
        if (*d0 != 0)
            return;
        chain->grow_left = (uint16_t)(chain->grow_left - 1);
        return;
    }
    chain->extension = (int32_t)((uint32_t)chain->extension + *d0);
    if (chain->extension < 0)
        return;   /* not grown a link's worth yet */
    if (chain->grow_left != 0) {
        ToyChain_AddLink(d0, a0, a1, chain, vm, self);
        if (*d0 != 0) {
            chain->extension = 0;   /* the pool is full */
            return;
        }
        shorten(chain);
        chain->grow_left = (uint16_t)(chain->grow_left - 1);
        if (chain->grow_left != 0)
            return;
        chain->grow_speed = chain->regrow_speed;   /* the initial links are done */
        return;
    }
    uint16_t count = vm->var[1];   /* +$24: the link count */
    *d0 = (*d0 & 0xFFFF0000u) | count;
    if ((int16_t)count >= (int16_t)chain->links_max) {
        chain->extension = 0;   /* complete */
        return;
    }
    ToyChain_AddLink(d0, a0, a1, chain, vm, self);
    if (*d0 != 0) {
        chain->extension = 0;
        return;
    }
    shorten(chain);
}
