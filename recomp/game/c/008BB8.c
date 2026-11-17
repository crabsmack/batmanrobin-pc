#include "game.h"

/* VIntObj_NewIdle ($008BB8-$008BF5) and, inside it, VIntObj_IdleHandler (its closing rts). */

/* VIntObj_NewIdle - VIntObj_New with the handler fixed to VIntObj_IdleHandler (this routine's
 * own rts), so the object does nothing until the caller stores its real handler.
 * In:  none
 * Out: result = 0, obj = the object (sign-extended handle), last = the previous last V-int
 *      object (a0), tail = g_VIntListTail (a1); or result = -1 with the rest unchanged when the
 *      pool is empty (g_ObjPoolFree clamped to 0); the condition codes are tst.l of result
 * Evidence: the VIntObj_New pop and link with move.l #VIntObj_IdleHandler,(a6); callers
 *           VM6_StartClearJob (then its own handler), the map blit setup $74A8, $1C3F2
 */
void VIntObj_NewIdle(uint32_t *result, uint32_t *last_out, uint32_t *tail_out, uint32_t *obj)
{
    if (g_ObjPoolFree-- == 0) {    /* the subq borrowed: the pool was empty */
        g_ObjPoolFree = 0;
        *result = 0xFFFFFFFFu;
        return;
    }
    g_VIntOff = (uint8_t)(g_VIntOff + 1);
    ObjNode *node = dk::handle<ObjNode>((int16_t)(uint16_t)g_ObjPoolHead);
    g_ObjPoolHead = (uint16_t)(int16_t)((ObjFreeBlock *)node)->next_free;
    node->handler = at_VIntObj_IdleHandler;
    ObjNode *tail = (ObjNode *)g_VIntListTail;
    ObjNode *last = dk::handle<ObjNode>(tail->prev);
    tail->prev = dk::handle_of(node);
    last->next = dk::handle_of(node);
    node->prev = dk::handle_of(last);
    node->next = dk::handle_of(tail);
    g_VIntOff = (uint8_t)(g_VIntOff - 1);
    *last_out = ADDR68(last);
    *tail_out = ADDR68(tail);
    *obj = ADDR68(node);
    *result = 0;
}

/* VIntObj_IdleHandler - the do-nothing V-int list handler VIntObj_NewIdle gives a new object.
 * In:  obj (a6, unused)
 * Out: none
 * Evidence: the rts at $008BEC; move.l #VIntObj_IdleHandler,(a6) in VIntObj_NewIdle. V-int code:
 *           proven by D2 only
 */
void VIntObj_IdleHandler(void)
{
}
