#include "game.h"

/* Obj_NewMultiAfter - Obj_NewMulti linked right after the running object instead of before the main
 * list's tail: pops `blocks` pool blocks, makes the first the object (handler) between `obj` and its
 * next node, then Obj_NewMulti's handle and clear runs.
 * In:  handler (a0); blocks (d7: the pool count takes its low byte); obj = the object to link after (a6)
 * Out: d0 = 0, obj = the new object (a6), handler_next = the node after it (a0), a1 = obj +8, a2 = the
 *      next free block; then Obj_NewMulti_FillHandles (d1, d7, a1, a2, g_ObjPoolHead). Short of blocks:
 *      Obj_NewMultiFail (d0 = -1)
 * Evidence: sub.b d7,(g_ObjPoolFree).w / bcs.w Obj_NewMultiFail / movea.w a6,a1 / movea.w
 *           (g_ObjPoolHead).w,a6 / movea.w (a6),a2 / move.l a0,(a6) / movea.w $6(a1),a0 / move.w a6,$6(a1) /
 *           move.w a6,$4(a0) / move.w a1,$4(a6) / move.w a0,$6(a6) / lea $8(a6),a1 / moveq #0,d0 /
 *           bra.w Obj_NewMulti_FillHandles; bsr target of Obj_SpawnAfter ($008F40); no corpus run reaches
 *           it (raw-stubs 2026-10-30); the contract is Obj_NewMulti's; 00899A.syn checks it
 */
void Obj_NewMultiAfter(uint32_t *d0, uint16_t *d1, uint16_t *blocks, uint32_t *handler_next, uint32_t *a1,
                       uint32_t *a2, uint32_t *obj)
{
    uint8_t want = (uint8_t)*blocks;
    bool borrow = g_ObjPoolFree < want;
    g_ObjPoolFree = (uint8_t)(g_ObjPoolFree - want);
    if (borrow) {
        dk::jump(at_Obj_NewMultiFail);
        return;
    }
    ObjNode *prev = dk::handle<ObjNode>((int16_t)(uint16_t)*obj);           /* movea.w a6,a1 */
    ObjNode *node = dk::handle<ObjNode>((int16_t)(uint16_t)g_ObjPoolHead);
    *a2 = (uint32_t)(int32_t)(int16_t)((ObjFreeBlock *)node)->next_free;
    node->handler = *handler_next;
    ObjNode *next = dk::handle<ObjNode>(prev->next);
    prev->next = dk::handle_of(node);
    next->prev = dk::handle_of(node);
    node->prev = dk::handle_of(prev);
    node->next = dk::handle_of(next);
    *handler_next = ADDR68(next);
    *obj = ADDR68(node);
    *a1 = ADDR68(&node->blocks[0]);
    *d0 = 0;
    dk::jump(at_Obj_NewMulti_FillHandles);
}
