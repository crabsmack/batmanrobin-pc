#include "game.h"

enum {
    CHAIN_MODE_SHOOTABLE = 1 << 1,   /* VM variable 0 bit: the links have body boxes */
    CHAIN_LINK_HALF = 8,             /* a link's body box: 16x16 around its position */
};

/* ToyChain_PlaceLinkBoxes - when variable 0 bit 1 is set, gives every link a 16x16 body box around
 * its position with the chain's energy, and links it into the enemy box list if it is on none
 * (the links can be shot)
 * In:  chain = the chain block (a2); vm = the chain's VM block (a4: +$22 variable 0, +$24 the
 *      count); self = the chain object (a6: +$52 the energy)
 * Out: each link box's energy and rectangle; d4-d7 = the last rectangle, d3 = $FFFF after the
 *      loop, a1 = past the link list (low words; nothing when the bit is clear)
 * Evidence: move.w $22(a4),d0 / andi.w #2 / beq / move.w $24(a4),d3 / subq.w #1 / bmi / lea $34(a2),a1
 *           / movea.w (a1)+,a0 / move.w $52(a6),$1E(a0) / subq/addq #8 around $44/$46 / movem.w
 *           d4-d7,$12(a0) / tst.w 2(a0) / bne / jsr Hitbox_LinkEnemyList / dbra d3; called by
 *           ToyChain_Update; TAS 6579 frames
 */
void ToyChain_PlaceLinkBoxes(uint16_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint16_t *d7, uint32_t *a1,
                             ToyChainBlock *chain, const Vm2Block *vm, const Vm2Obj *self)
{
    if (!(vm->var[0] & CHAIN_MODE_SHOOTABLE))
        return;
    *d3 = (uint16_t)(vm->var[1] - 1);   /* +$24: the link count */
    if ((int16_t)*d3 < 0)
        return;
    *a1 = dk::addr(chain->link);
    do {
        PTR(const s16) slot;
        slot.v = *a1;
        *a1 += 2;
        ToyChainLink *link = dk::handle<ToyChainLink>(*slot);
        link->box.energy = self->energy;
        *d4 = (uint16_t)(link->x - CHAIN_LINK_HALF);
        *d5 = (uint16_t)(link->y - CHAIN_LINK_HALF);
        *d6 = (uint16_t)(link->x + CHAIN_LINK_HALF);
        *d7 = (uint16_t)(link->y + CHAIN_LINK_HALF);
        link->box.left = (int16_t)*d4;
        link->box.top = (int16_t)*d5;
        link->box.right = (int16_t)*d6;
        link->box.bottom = (int16_t)*d7;
        if (link->box.prev == 0) {   /* on no list yet */
            dk::Flags linked{};      /* Hitbox_LinkEnemyList's flags stay in the SR for the caller */
            Hitbox_LinkEnemyList(dk::addr(link), *a1, &linked);
        }
    } while ((*d3)-- != 0);
}
