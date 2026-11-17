#include "game.h"

enum {
    BOX_STRUCK_BIT = 1 << 0,       /* HitBox.flags: struck this frame (Hitbox_Strike sets it) */
};

/* ToyChain_TakeLinkHits - switch native: collects the hits on the chain's links: clears each link's
 * struck bit, takes its damage off the chain's energy and remembers the box in g_StruckBox; when
 * the energy goes negative it is reset to one link's energy (the script then removes a link)
 * In:  vm = the chain's VM block (a4: +$24 the link count); self = the chain object (a6: +$E the
 *      chain block, +$52 the energy)
 * Out: d0 = 0 not hit, 1 hit, 2 a link's worth of energy used up; d1 = the last damage taken;
 *      d3 = $FFFF after the loop; a1 = past the link list, or the chain block when the energy ran
 *      out
 * Evidence: moveq #0,d0 / move.w $24(a4),d3 / subq.w #1 / bmi / lea $34($E(a6)),a1 / movea.w
 *           (a1)+,a0 / bclr #0,$3C(a0) / beq / moveq #1 / move.w $20(a0),d1 / clr.w $20(a0) / sub.w
 *           d1,$52(a6) / move.l a0,g_StruckBox / dbra d3 / tst.w $52(a6) / bpl / move.w $20(a1),$52(a6)
 *           / moveq #2; Op2_CallNativeRelSwitch at $0379EE, $037BA8, $037DE8, $038040; TAS 6492 frames
 */
void ToyChain_TakeLinkHits(uint32_t *d0, uint16_t *d1, uint16_t *d3, uint32_t *a1, const Vm2Block *vm,
                           Vm2Obj *self)
{
    *d0 = 0;
    *d3 = (uint16_t)(vm->var[1] - 1);   /* +$24: the link count */
    if ((int16_t)*d3 < 0)
        return;
    ToyChainBlock *chain = toy_chain_block(self);
    *a1 = dk::addr(chain->link);
    do {
        PTR(const s16) slot;
        slot.v = *a1;
        *a1 += 2;
        ToyChainLink *link = dk::handle<ToyChainLink>(*slot);
        bool struck = (link->box.flags & BOX_STRUCK_BIT) != 0;
        link->box.flags = (uint8_t)(link->box.flags & ~BOX_STRUCK_BIT);
        if (struck) {
            *d0 = 1;
            *d1 = link->box.damage;
            link->box.damage = 0;
            self->energy = (int16_t)(self->energy - (int16_t)*d1);
            g_StruckBox = dk::addr(link);
        }
    } while ((*d3)-- != 0);
    if (self->energy >= 0)
        return;
    *a1 = dk::addr(chain);
    self->energy = (int16_t)chain->link_energy;
    *d0 = 2;
}
