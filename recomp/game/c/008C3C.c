#include "game.h"

/* a 68000 address held in a register, as a pointer (PTR(T): the 68000 value kept) */
template <class T> static T *ptr(uint32_t a)
{
    PTR(T) p;
    p.v = a;
    return p;
}

/* Hitbox_LinkPlayerList - appends a collision box to the end of g_PlayerBoxList: after the last
 * box (g_PlayerBoxLast, the tail sentinel's prev link), before the tail sentinel.
 * In:  a0 = the box (a pool block); a1 = only parked in usp and restored
 * Out: box next = the tail sentinel, prev = the old last node; that node's next = the box;
 *      g_PlayerBoxLast = the box; ccr = move.w #$DEB0's (N set, Z V C clear)
 * Evidence: move.l a1,usp / movea.w g_PlayerBoxLast,a1 / move.w a0,g_PlayerBoxLast /
 *           move.w a0,(a1) / move.w a1,$2(a0) / move.w #$DEB0,(a0) / move.l usp,a1; bra from
 *           Hitbox_LinkByKind (kinds 0/4). The tail sentinel is written as the constant $DEB0
 *           (g_PlayerBoxTail's address), so the flags are those of a negative word
 */
void Hitbox_LinkPlayerList(uint32_t a0, uint32_t a1, dk::Flags *ccr)
{
    (void)a1;
    HitBox *box = ptr<HitBox>(a0);
    int16_t last = (int16_t)g_PlayerBoxLast;
    g_PlayerBoxLast = (uint16_t)a0;
    dk::handle<HitBox>(last)->next = (int16_t)(uint16_t)a0;
    box->prev = last;
    box->next = dk::handle_of(g_PlayerBoxTail);
    *ccr = dk::Flags::test((uint16_t)box->next, 2, ccr->x);
}
