#include "game.h"

/* VIntObj_ListEnd ($008B22-$008B25): the V-int object list's tail sentinel handler. */

/* VIntObj_ListEnd - the handler of the V-int list's tail sentinel: drops the jsr's return
 * address, so the return ends VIntObj_RunList's walk and goes back to the V-int.
 * In:  called by VIntObj_RunList's jsr (a5)
 * Out: returns to VIntObj_RunList's caller
 * Evidence: addq.l #4,a7 / rts; VIntObj_InitList stores it as g_VIntListTail's handler. V-int
 *           code: proven by D2 only
 */
void VIntObj_ListEnd(void)
{
    dk::drop_return();
}
