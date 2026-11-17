#include "game.h"

/* VIntObj_New ($008B7E-$008BB7). */

/* VIntObj_New - creates a one-block object on the V-int list: pops a pool block, sets its handler
 * and links it before g_VIntListTail, with g_VIntOff raised around the edit so a V-int in the
 * middle of it is skipped.
 * In:  handler (a0)
 * Out: result = 0, obj = the object (sign-extended handle), tail = g_VIntListTail (a1); or
 *      result = -1 with the rest unchanged when the pool is empty (g_ObjPoolFree clamped to 0);
 *      the condition codes are tst.l of result (moveq); a0 clobbered
 * Evidence: subq.b #1,(g_ObjPoolFree).w / bcs / addq.b #1,(g_VIntOff).w / the Obj_New pop and
 *           link on g_VIntListTail / subq.b #1,(g_VIntOff).w; callers the stage setups $1FB3A,
 *           $1FC50, $1FD76, $1FECC (Layer_FlushTileQueueIfIdle, Stage1_VIntFacade)
 */
void VIntObj_New(uint32_t *result, uint32_t handler, uint32_t *tail_out, uint32_t *obj)
{
    if (g_ObjPoolFree-- == 0) {    /* the subq borrowed: the pool was empty */
        g_ObjPoolFree = 0;
        *result = 0xFFFFFFFFu;
        return;
    }
    g_VIntOff = (uint8_t)(g_VIntOff + 1);
    ObjNode *node = dk::handle<ObjNode>((int16_t)(uint16_t)g_ObjPoolHead);
    g_ObjPoolHead = (uint16_t)(int16_t)((ObjFreeBlock *)node)->next_free;
    node->handler = handler;
    ObjNode *tail = (ObjNode *)g_VIntListTail;
    ObjNode *last = dk::handle<ObjNode>(tail->prev);
    tail->prev = dk::handle_of(node);
    last->next = dk::handle_of(node);
    node->prev = dk::handle_of(last);
    node->next = dk::handle_of(tail);
    g_VIntOff = (uint8_t)(g_VIntOff - 1);
    *tail_out = ADDR68(tail);
    *obj = ADDR68(node);
    *result = 0;
}
