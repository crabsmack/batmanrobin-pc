#include "game.h"

/* Obj_ListEnd ($0088BE-$0088C1): the main object list's tail sentinel handler. */

/* Obj_ListEnd - the handler of the main list's tail sentinel: drops the jsr's return address, so
 * the return ends Obj_RunList's walk and goes to its caller.
 * In:  called by Obj_RunList's jsr (a5)
 * Out: returns to Obj_RunList's caller
 * Evidence: addq.l #4,a7 / rts; Obj_InitList stores it as g_ObjListTail's handler
 */
void Obj_ListEnd(void)
{
    dk::drop_return();
}
