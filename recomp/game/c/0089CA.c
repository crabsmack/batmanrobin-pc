#include "game.h"

/* Obj_NewClear ($0089CA-$008A07): a one-block object on the main list, handle words zeroed. */

/* Obj_NewClear - creates a one-block object on the main list: pops a pool block, sets its
 * handler, links it before the tail sentinel and zeroes its handle words +8..+$17.
 * In:  handler (a0)
 * Out: result = 0 and obj = the object (sign-extended handle); or result = -1, obj unchanged,
 *      when the pool is empty (g_ObjPoolFree clamped to 0); the condition codes are tst.l of
 *      result; a0/a1 clobbered
 * Evidence: the ObjPool_Alloc pop, move.l a0,(a6), the Obj_New link, lea $8(a6),a0 /
 *           4 x move.l d0,(a0)+; callers the player setup $99D4 (Player_JoinOrSpawn),
 *           Stage1_StartRubbleRain, $49798
 */
void Obj_NewClear(uint32_t *result, uint32_t handler, uint32_t *obj)
{
    if (g_ObjPoolFree-- == 0) {    /* the subq borrowed: the pool was empty */
        g_ObjPoolFree = 0;
        *result = 0xFFFFFFFFu;
        return;
    }
    ObjNode *node = dk::handle<ObjNode>((int16_t)(uint16_t)g_ObjPoolHead);
    g_ObjPoolHead = (uint16_t)(int16_t)((ObjFreeBlock *)node)->next_free;
    node->handler = handler;
    ObjNode *tail = (ObjNode *)g_ObjListTail;
    ObjNode *last = dk::handle<ObjNode>(tail->prev);
    tail->prev = dk::handle_of(node);
    last->next = dk::handle_of(node);
    node->prev = dk::handle_of(last);
    node->next = dk::handle_of(tail);
    for (int i = 0; i < OBJ_HANDLE_SLOTS; i++)
        node->blocks[i] = 0;
    *obj = ADDR68(node);
    *result = 0;
}
