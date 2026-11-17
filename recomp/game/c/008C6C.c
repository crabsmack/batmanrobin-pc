#include "game.h"

/* a 68000 address held in a register, as a pointer (PTR(T): the 68000 value kept) */
template <class T> static T *ptr(uint32_t a)
{
    PTR(T) p;
    p.v = a;
    return p;
}

/* Hitbox_Unlink - unlinks a collision box from whichever list it is on: its prev node's next link
 * skips it, its next node's prev link points back past it. The box keeps its own links.
 * In:  a0 = the box; a1 = only parked in usp and restored
 * Out: prev->next = box->next, next->prev = box->prev
 * Evidence: move.l a1,usp / movea.w $2(a0),a1 / move.w (a0),(a1) / movea.w (a0),a1 /
 *           move.w $2(a0),$2(a1) / move.l usp,a1; callers free the box after it ($3858, $5406,
 *           $6156 then ObjPool_Free; $4F4E, $53D4, $372C0)
 */
void Hitbox_Unlink(uint32_t a0, uint32_t a1)
{
    (void)a1;
    const HitBox *box = ptr<const HitBox>(a0);
    dk::handle<HitBox>(box->prev)->next = box->next;
    dk::handle<HitBox>(box->next)->prev = box->prev;
}
