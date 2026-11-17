#include "game.h"

/* Hitbox_InitLists - empties both collision box lists: each head links straight to its tail
 * sentinel, whose prev link (g_PlayerBoxLast / g_EnemyBoxLast) points back at the head and whose
 * next link is 0 (the end mark the box scans stop at).
 * In:  none
 * Out: g_PlayerBoxList = &g_PlayerBoxTail, g_PlayerBoxLast = &g_PlayerBoxList, tail next = 0; the
 *      same for the enemy list
 * Evidence: lea g_PlayerBoxList,a0 / lea g_PlayerBoxTail,a1 / move.w a1,(a0) / move.w a0,$2(a1) /
 *           clr.w (a1), then the enemy list; only caller the scene init $9AAC (after ObjPool_Init)
 */
void Hitbox_InitLists(void)
{
    HitBox *head = (HitBox *)&g_PlayerBoxList, *tail = (HitBox *)g_PlayerBoxTail;
    head->next = dk::handle_of(tail);
    tail->prev = dk::handle_of(head);
    tail->next = 0;

    head = (HitBox *)&g_EnemyBoxList;
    tail = (HitBox *)g_EnemyBoxTail;
    head->next = dk::handle_of(tail);
    tail->prev = dk::handle_of(head);
    tail->next = 0;
}
