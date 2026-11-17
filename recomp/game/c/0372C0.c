#include "game.h"

/* ToyChain_RemoveLastLink - removes the newest link of a chain: sets the link-lost bit, stores the
 * link's position as the anchor (the integer parts of the saved position; Spawn_AtAnchor then
 * puts a single figure there), releases its frame, unlinks its box and frees the block
 * In:  vm = the chain's VM block (a4: +$24 the link count, +$22 variable 0); self = the chain
 *      object (a6: +$E the chain block)
 * Out: d0 = 0 removed, 1 none left (tst: Z set = removed); d2 = the released frame; a1 = the link
 *      (sign-extended handle)
 * Evidence: move.w $24(a4),d0 / subq.w #1 / bmi / move.w d0,$24(a4) / ori.w #$10,$22(a4) / add.w
 *           d0,d0 / movea.w $E(a6),a1 / movea.w $34(a1,d0.w),a1 / move.w $44/$46(a1),$46/$4A(a6) /
 *           move.w $42(a1),d2 / jsr Spr_ReleaseFrame / tst.w 2(a0) / beq / jsr Hitbox_Unlink / jsr
 *           ObjPool_Free / moveq #0 (moveq #1 when none); Op2_CallNativeRelSwitch in the shedding
 *           routines $03887C-$038912; TAS 504 frames. The prototype adds tst(d0.l): ccout Z is the
 *           moveq's
 */
void ToyChain_RemoveLastLink(uint32_t *d0, uint16_t *d2, uint32_t *a1, Vm2Block *vm, Vm2Obj *self)
{
    uint16_t last = (uint16_t)(vm->var[1] - 1);   /* +$24: the link count */
    *d0 = (*d0 & 0xFFFF0000u) | last;
    if ((int16_t)last < 0) {
        *d0 = 1;
        return;
    }
    vm->var[1] = last;
    vm->var[0] = (uint16_t)(vm->var[0] | TOYCHAIN_LOST_LINK);
    uint16_t offset = (uint16_t)(last * 2);
    *d0 = (*d0 & 0xFFFF0000u) | offset;
    ToyChainBlock *chain = toy_chain_block(self);
    ToyChainLink *link = dk::handle<ToyChainLink>(chain->link[last]);   /* $34(a1,d0.w): d0 = last * 2 */
    *a1 = dk::addr(link);
    /* move.w into the saved longs' high words */
    self->saved_x = (int32_t)((uint32_t)(uint16_t)link->x << 16 | ((uint32_t)self->saved_x & 0xFFFFu));
    self->saved_y = (int32_t)((uint32_t)(uint16_t)link->y << 16 | ((uint32_t)self->saved_y & 0xFFFFu));
    *d2 = link->frame;
    uint16_t d0w = (uint16_t)*d0;
    uint32_t a0 = 0;
    Spr_ReleaseFrame(&d0w, d2, &a0);
    a0 = *a1;
    if (link->box.prev != 0)
        Hitbox_Unlink(a0, *a1);
    ObjPool_Free(a0);
    *d0 = 0;
}
