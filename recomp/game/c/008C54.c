#include "game.h"

/* a 68000 address held in a register, as a pointer (PTR(T): the 68000 value kept) */
template <class T> static T *ptr(uint32_t a)
{
    PTR(T) p;
    p.v = a;
    return p;
}

/* Hitbox_LinkEnemyList - appends a collision box to the end of g_EnemyBoxList: after the last
 * box (g_EnemyBoxLast, the tail sentinel's prev link), before the tail sentinel.
 * In:  a0 = the box (a pool block); a1 = only parked in usp and restored
 * Out: box next = the tail sentinel, prev = the old last node; that node's next = the box;
 *      g_EnemyBoxLast = the box; ccr = move.w #$DEB8's (N set, Z V C clear)
 * Evidence: move.l a1,usp / movea.w g_EnemyBoxLast,a1 / move.w a0,g_EnemyBoxLast /
 *           move.w a0,(a1) / move.w a1,$2(a0) / move.w #$DEB8,(a0) / move.l usp,a1; callers
 *           Hitbox_LinkByKind_Enemy (kinds 8/$C), Op4_BodyBox $66E2, $374C2
 */
void Hitbox_LinkEnemyList(uint32_t a0, uint32_t a1, dk::Flags *ccr)
{
    (void)a1;
    HitBox *box = ptr<HitBox>(a0);
    int16_t last = (int16_t)g_EnemyBoxLast;
    g_EnemyBoxLast = (uint16_t)a0;
    dk::handle<HitBox>(last)->next = (int16_t)(uint16_t)a0;
    box->prev = last;
    box->next = dk::handle_of(g_EnemyBoxTail);
    *ccr = dk::Flags::test((uint16_t)box->next, 2, ccr->x);
}
