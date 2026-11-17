#include "game.h"

#if DK_MODE_NATIVE
extern "C" void hf_spr_owner(uint32_t object);   /* platform/native/present.h: high_fps's sprite owners */
#endif

/* Obj_RunList ($0087B6-$0088BD).
 *
 * The original walks the list with 32 unrolled `movea.w 6(a6),a6 / movea.l (a6),a5 / jsr (a5)`
 * and a bra back; it never ends by itself: the tail sentinel's handler Obj_ListEnd drops its
 * jsr's return address, so its rts returns from Obj_RunList. Obj_ListEnd is C too, and a C call
 * of C that returns past its caller comes back with A7 off by 4 (the bridge's check: "call of
 * $0088BE from C came back with A7 $FFFFFFF4"), so the C returns when it reaches that handler,
 * which is what its call does. The handlers get and leave every register: each one starts with
 * what the one before it left. */

/* Obj_RunList - calls the per-frame handler of every object on the main object list, in list
 * order, with a6 = the object, until the tail sentinel (Obj_ListEnd) returns for it.
 * In:  every register (the first handler gets them)
 * Out: every register as the last handlers left them; a6 = the tail sentinel, a5 = Obj_ListEnd
 * Evidence: lea (g_ObjListHead).w,a6, 32 x (movea.w $6(a6),a6 / movea.l (a6),a5 / jsr (a5)),
 *           bra.w loc_0087BA; callers the gameplay frame loops (Frame_RunObjects $9BC0, $32F14,
 *           $33274, $401DA, ...)
 */
void Obj_RunList(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                 uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5,
                 uint32_t *a6)
{
    *a6 = ADDR68(g_ObjListHead);
    for (;;) {
        PTR(ObjNode) node;
        node.v = *a6;
        *a6 = (uint32_t)(int32_t)(int16_t)node->next;
        node.v = *a6;
        *a5 = node->handler;
        if (*a5 == at_Obj_ListEnd) {
#if DK_MODE_NATIVE
            hf_spr_owner(0);     /* high_fps: no object runs now */
#endif
            return;              /* its jsr: addq.l #4,a7 / rts returns from here */
        }
#if DK_MODE_NATIVE
        hf_spr_owner(*a6);       /* high_fps (phase 6 R3): the display-list entries carved next are this object's */
#endif
        ObjHandlerFn(*a5, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
    }
}
