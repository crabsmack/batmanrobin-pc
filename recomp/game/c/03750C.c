#include "game.h"

enum {
    CHAIN_MODE_CHARGED = 1 << 2,   /* VM variable 0 bit: charged, the links hurt on touch */
    CHAIN_LINK_HALF = 8,           /* a link's touch box: 16x16 around its position */
};

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }
static uint16_t ror_word(uint16_t v) { return (uint16_t)(v >> 1 | v << 15); }
/* dbra: the low word - 1, looping until it passes 0 (-1) */
static bool dbra(uint32_t *r) { uint16_t v = (uint16_t)((uint16_t)*r - 1); set_low_word(r, v); return v != 0xFFFFu; }

/* the box searches return Z clear with a2 = the box found, or Z set with a2 = the list's tail
   sentinel: the one node whose next link is 0, so the link says which (beq after the jsr) */
static bool found_box(uint32_t a2)
{
    return dk::handle<HitBox>((int16_t)(uint16_t)a2)->next != 0;
}

/* ToyChain_LinkTouchAttack - when variable 0 bit 2 is set (charged), tests half of the links each
 * frame (the sprite block's pattern, $AAAA from ToyChain_SpawnInline, rotated a step per frame)
 * against the player boxes and runs each hit: the struck box gets the chain's owner id, then the
 * link's hit callback and the player box's own callback run; also steps the chain's blink
 * In:  chain = the chain block (a2); vm = the chain's VM block (a4: +$22 variable 0, +$24 the
 *      count); spr = its sprite block (a5: +$22 the touch pattern, +$24 its working copy); a6 =
 *      the chain object; a3 passed to the callbacks
 * Out: the chain block's blink timer and pattern stepped; player boxes hit (through the
 *      callbacks); d0-d7 / a1 as the searches and the callbacks left them (d3 the loop count,
 *      $FFFF after it; d4-d7 the last touch rectangle's low words). The callbacks may change a4 /
 *      a6: the loop goes on with theirs, as on the 68000 (the contract lists them as clobbers)
 * Evidence: move.w $22(a4),d0 / andi.w #4 / beq / move.w $24(a4),d3 / subq.w #1 / bmi / subq.w
 *           #1,$1C(a2) / bpl / move.w $1E(a2),$1C(a2) / ror.w $18(a2) / move.l a2,-(a7) / lea
 *           $34(a2),a1 / ror.w $22(a5) / move.w $22(a5),$24(a5) / movea.w (a1)+,a0 / ror.w $24(a5)
 *           / bcc / the rectangle / jsr Hitbox_FindPlayerBox / beq / move.w $58(a6),$1A(a2) /
 *           movea.l $3E(a0),a1 / jsr (a1) / movea.l $30(a2),a1 / jsr (a1) / jsr Hitbox_FindNextBox
 *           / bne / dbra d3 / movea.l (a7)+,a2; called by ToyChain_Update; TAS 6579 frames. The
 *           two calls through pointers are HitCallback (the hit callbacks' union convention); the
 *           stack pushes of a2 / a1 only keep them over the calls: the C keeps them in locals
 */
void ToyChain_LinkTouchAttack(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                              uint32_t *d6, uint32_t *d7, uint32_t *a1, ToyChainBlock *chain, uint32_t a3,
                              uint32_t a4, Vm2Sprite *spr, uint32_t a6)
{
    PTR(const Vm2Block) vm;
    vm.v = a4;
    uint16_t mode = vm->var[0];
    set_low_word(d0, (uint16_t)(mode & CHAIN_MODE_CHARGED));
    if (!(mode & CHAIN_MODE_CHARGED))
        return;
    set_low_word(d3, (uint16_t)(vm->var[1] - 1));   /* +$24: the link count */
    if ((int16_t)(uint16_t)*d3 < 0)
        return;
    chain->blink_timer = (int16_t)(chain->blink_timer - 1);
    if (chain->blink_timer < 0) {
        chain->blink_timer = (int16_t)chain->blink_reload;
        chain->blink_pattern = ror_word(chain->blink_pattern);
    }
    uint32_t links = dk::addr(chain->link);
    *a1 = links;
    spr->var[0] = (int16_t)ror_word((uint16_t)spr->var[0]);
    spr->var[1] = spr->var[0];
    do {
        PTR(const s16) slot;
        slot.v = links;
        links += 2;
        *a1 = links;
        uint32_t a0 = (uint32_t)(int32_t)*slot;   /* movea.w: the link */
        uint16_t pattern = (uint16_t)spr->var[1];
        spr->var[1] = (int16_t)ror_word(pattern);
        if (!(pattern & 1u))   /* bcc: this link waits for the other frame */
            continue;
        PTR(const ToyChainLink) link;
        link.v = a0;
        set_low_word(d4, (uint16_t)(link->x - CHAIN_LINK_HALF));
        set_low_word(d5, (uint16_t)(link->y - CHAIN_LINK_HALF));
        set_low_word(d6, (uint16_t)(link->x + CHAIN_LINK_HALF));
        set_low_word(d7, (uint16_t)(link->y + CHAIN_LINK_HALF));
        uint32_t a2 = 0;
        Hitbox_FindPlayerBox(d0, (uint16_t)*d4, (uint16_t)*d5, (uint16_t)*d6, (uint16_t)*d7, &a2);
        if (!found_box(a2))
            continue;
        for (;;) {
            PTR(HitBox) box;
            box.v = a2;
            PTR(const Vm2Obj) owner;
            owner.v = a6;
            box->struck_by = owner->owner_id;
            PTR(const ToyChainLink) hit_link;
            hit_link.v = a0;
            HitCallback(hit_link->hit, d0, d1, d2, d3, d4, d5, d6, d7, &a0, a2, a3, &a4, dk::addr(spr), &a6);
            HitCallback(box->on_hit, d0, d1, d2, d3, d4, d5, d6, d7, &a0, a2, a3, &a4, dk::addr(spr), &a6);
            *a1 = links;   /* movea.l (a7)+,a1 */
            HitBox *next = box;
            dk::Flags found{};
            Hitbox_FindNextBox(d0, *d4, *d5, *d6, *d7, &next, &found);
            a2 = dk::addr(next);
            if (found.z)
                break;
        }
    } while (dbra(d3));
}
