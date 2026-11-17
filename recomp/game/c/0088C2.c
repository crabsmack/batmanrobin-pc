#include "game.h"

/* Obj_InitList ($0088C2-$0088D9): empties the main object list. */

/* Obj_InitList - links the head node g_ObjListHead straight to the tail sentinel g_ObjListTail
 * and gives the sentinel the handler Obj_ListEnd, which ends Obj_RunList's walk.
 * In:  none
 * Out: head next = tail, tail prev = head, tail handler = Obj_ListEnd (a0/a1 clobbered)
 * Evidence: lea (g_ObjListHead).w,a0 / lea (g_ObjListTail).w,a1 / move.w a1,$6(a0) /
 *           move.w a0,$4(a1) / move.l #Obj_ListEnd,(a1); only caller the scene init $9AAC
 */
void Obj_InitList(void)
{
    ObjNode *head = (ObjNode *)g_ObjListHead;
    ObjNode *tail = (ObjNode *)g_ObjListTail;
    head->next = dk::handle_of(tail);
    tail->prev = dk::handle_of(head);
    tail->handler = at_Obj_ListEnd;
}
