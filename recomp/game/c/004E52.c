#include "game.h"

/* The hit dispatch ($004E52-$004E81): an attack box strikes a box; each box's hit callback runs. */

/* the d-registers, a3, a4 and a5 a hit callback reads and hands back, passed through */
#define HIT_REGS d0, d1, d2, d3, d4, d5, d6, d7

/* Hitbox_Strike - one hit: tags the struck box with the attacker's owner id, runs the attack box's
 * hit callback, then the struck box's.
 * In:  attack, struck = the boxes; a6 = the attacking object; the registers the callbacks read
 * Out: a1 = the struck box's callback, jumped to; whatever the attack box's callback hands back
 * Evidence: move.w $58(a6),$1A(a2) / movea.l $30(a0),a1 / jsr (a1) / movea.l $30(a2),a1 / jmp (a1);
 *           the struck box's callback was Native_NoOp in every corpus call (call-site log $4E62)
 */
void Hitbox_Strike(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                   uint32_t *d7, const HitBox *attack, uint32_t *a1, HitBox *struck, uint32_t a3, uint32_t *a4,
                   uint32_t a5, uint32_t *a6)
{
    PTR(const Vm2Obj) self;
    self.v = *a6;
    struck->struck_by = self->owner_id;
    *a1 = attack->on_hit;
    uint32_t a0 = dk::addr(attack);
    HitCallback(*a1, HIT_REGS, &a0, dk::addr(struck), a3, a4, a5, a6);
    *a1 = struck->on_hit;
    dk::jump(*a1);
}

/* Hitbox_StrikeAll - as Hitbox_Strike for every remaining overlapping box on the list
 * (Hitbox_FindNextBox), each struck box's callback a call.
 * In:  attack, struck = the first pair; a6 = the attacking object; the registers the callbacks read
 * Out: a1 = the last callback; whatever the callbacks hand back
 * Evidence: the Hitbox_Strike sequence with jsr (a1) for the struck box too, then bsr.w
 *           Hitbox_FindNextBox / bne.s back; the callbacks' a0 and a6 carry into the next round
 */
void Hitbox_StrikeAll(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                      uint32_t *d6, uint32_t *d7, const HitBox *attack, uint32_t *a1, HitBox *struck, uint32_t a3,
                      uint32_t *a4, uint32_t a5, uint32_t *a6)
{
    uint32_t a0 = dk::addr(attack);
    HitBox *box = struck;
    dk::Flags found{};
    do {
        PTR(const Vm2Obj) self;
        self.v = *a6;
        box->struck_by = self->owner_id;
        PTR(const HitBox) atk;
        atk.v = a0;
        *a1 = atk->on_hit;
        HitCallback(*a1, HIT_REGS, &a0, dk::addr(box), a3, a4, a5, a6);
        *a1 = box->on_hit;
        HitCallback(*a1, HIT_REGS, &a0, dk::addr(box), a3, a4, a5, a6);
        Hitbox_FindNextBox(d0, *d4, *d5, *d6, *d7, &box, &found);
    } while (!found.z);
}
