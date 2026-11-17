#include "game.h"

/* VIntObj_InitList ($008B26-$008B3D); the raw link / unlink after it ($008B3E-$008B7D) never
 * runs. */

/* VIntObj_InitList - empties the V-int object list: links g_VIntListHead straight to the tail
 * sentinel g_VIntListTail and gives the sentinel the handler VIntObj_ListEnd.
 * In:  none
 * Out: head next = tail, tail prev = head, tail handler = VIntObj_ListEnd (a0/a1 clobbered)
 * Evidence: lea (g_VIntListHead).w,a0 / lea (g_VIntListTail).w,a1 / move.w a1,$6(a0) /
 *           move.w a0,$4(a1) / move.l #VIntObj_ListEnd,(a1); only caller the scene init $9AAC
 */
void VIntObj_InitList(void)
{
    ObjNode *head = (ObjNode *)g_VIntListHead;
    ObjNode *tail = (ObjNode *)g_VIntListTail;
    head->next = dk::handle_of(tail);
    tail->prev = dk::handle_of(head);
    tail->handler = at_VIntObj_ListEnd;
}
