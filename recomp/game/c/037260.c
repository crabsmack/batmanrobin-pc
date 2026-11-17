#include "game.h"

/* ToyChain_AddLink - adds one link to a chain: a pool block used as a collision box (the no-op hit
 * callback Native_NoOp, owner a6 with its id and serial, the chain's link hit callback and its
 * damage / knockback arguments), its handle stored after the last one at +$34 of the chain block
 * In:  chain = the chain block (a2); vm = the chain's VM block (a4: +$24 the link count); self =
 *      the chain object (a6); a0 passed to ObjPool_Alloc
 * Out: d0 = 0 (Z set) and the count + 1, a1 = past the new handle; or ObjPool_Alloc's -1 (Z clear)
 *      when the pool is empty, a1 = where the handle would go
 * Evidence: move.w $24(a4),d0 / add.w d0,d0 / lea $34(a2,d0.w),a1 / jsr ObjPool_Alloc / bne /
 *           move.l #$82C,$30(a0) / clr.w 2, $3C, $20 / move.w a6,$2E(a0) / move.w $58(a6),$10(a0) /
 *           move.w $56(a6),d0 / move.w d0,4(a0) / clr.w $42(a0) / move.l $26(a2),$3E(a0) /
 *           move.w $22/$24(a2),$34/$36(a0) / move.w a0,(a1)+ / addq.w #1,$24(a4) / moveq #0,d0;
 *           callers ToyChain_Grow; TAS 561 frames. The prototype adds tst(d0.l): ccout Z is the
 *           moveq's, or ObjPool_Alloc's (itself tst of d0)
 */
void ToyChain_AddLink(uint32_t *d0, uint32_t a0, uint32_t *a1, ToyChainBlock *chain, Vm2Block *vm, Vm2Obj *self)
{
    uint16_t count = vm->var[1];   /* +$24: the link count */
    uint16_t offset = (uint16_t)(count * 2);
    *d0 = (*d0 & 0xFFFF0000u) | offset;
    *a1 = dk::addr(&chain->link[count]);   /* lea $34(a2,d0.w): d0 = count * 2 */
    ObjPool_Alloc(d0, &a0);
    if (*d0 != 0)
        return;   /* the pool is empty */
    PTR(ToyChainLink) link;
    link.v = a0;
    link->box.on_hit = at_Native_NoOp;
    link->box.prev = 0;
    link->box.flags = 0;   /* clr.w $3C: the flags and the byte after them */
    link->box.unk_3D = 0;
    link->box.damage = 0;
    link->box.owner = dk::handle_of(self);
    link->box.id = self->owner_id;
    uint16_t serial = self->serial;
    *d0 = (*d0 & 0xFFFF0000u) | serial;
    link->box.serial = serial;
    link->frame = 0;
    link->hit = chain->link_hit;
    link->box.hit_damage = chain->link_damage;
    link->box.hit_knockback = (int16_t)chain->link_knockback;
    PTR(s16) slot;
    slot.v = *a1;
    *slot = (int16_t)(uint16_t)a0;
    *a1 += 2;
    vm->var[1] = (uint16_t)(count + 1);
    *d0 = 0;
}
