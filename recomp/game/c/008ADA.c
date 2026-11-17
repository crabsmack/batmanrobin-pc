#include "game.h"

/* VIntObj_RunList ($008ADA-$008B21).
 *
 * The same walk as Obj_RunList on the V-int list (8 unrolled units and a bra back): the tail
 * sentinel's handler VIntObj_ListEnd drops its jsr's return address, so its rts returns from
 * VIntObj_RunList; it is C too, so the C returns when it reaches that handler (a C call of C
 * returning past its caller is the bridge's A7 error, see Obj_RunList). The handlers get and
 * leave every register:
 * each starts with what the one before it left (locals here for the registers the measured
 * contract doesn't pass in or out). */

/* VIntObj_RunList - calls the handler of every object on the V-int object list, in list order,
 * with a6 = the object, until the tail sentinel (VIntObj_ListEnd) returns for it.
 * In:  d1-d7, a0-a2 (the first handler gets them)
 * Out: a0-a2 as the handlers left them (d0-d7, a3-a6 clobbered: no caller reads them)
 * Evidence: lea (g_VIntListHead).w,a6, 8 x (movea.w $6(a6),a6 / movea.l (a6),a5 / jsr (a5)),
 *           bra.w loc_008ADE; VInt_Handler's full path ($7736) and VInt_BusyPath, every V-int.
 *           V-int code: proven by D2 only
 */
void VIntObj_RunList(uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7,
                     uint32_t *a0, uint32_t *a1, uint32_t *a2)
{
    uint32_t d0 = 0, a3 = 0, a4 = 0, a5 = 0;
    uint32_t a6 = ADDR68(g_VIntListHead);
    for (;;) {
        PTR(ObjNode) node;
        node.v = a6;
        a6 = (uint32_t)(int32_t)(int16_t)node->next;
        node.v = a6;
        a5 = node->handler;
        if (a5 == at_VIntObj_ListEnd)
            return;              /* its jsr: addq.l #4,a7 / rts returns from here */
        ObjHandlerFn(a5, &d0, &d1, &d2, &d3, &d4, &d5, &d6, &d7, a0, a1, a2, &a3, &a4, &a5, &a6);
    }
}
