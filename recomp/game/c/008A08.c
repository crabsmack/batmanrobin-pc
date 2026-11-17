#include "game.h"

/* Obj_New ($008A08-$008A39) and Obj_UnlinkFreeWithBlocks ($008A3A-$008A47), which falls into
 * Obj_FreeWithBlocks (the next function file). */

/* Obj_New - creates a one-block object on the main list: pops a pool block, sets its handler and
 * links it before the tail sentinel; the rest of the block is left as it was.
 * In:  handler (a0)
 * Out: result = 0, obj = the object (sign-extended handle), last = the previous last object (a0),
 *      tail = g_ObjListTail (a1); or result = -1 with the rest unchanged when the pool is empty
 *      (g_ObjPoolFree clamped to 0); the condition codes are tst.l of result (moveq)
 * Evidence: subq.b #1,(g_ObjPoolFree).w / bcs / movea.w (g_ObjPoolHead).w,a6 /
 *           move.w (a6),(g_ObjPoolHead).w / move.l a0,(a6) / lea (g_ObjListTail).w,a1 /
 *           movea.w $4(a1),a0 / move.w a6,$4(a1) / move.w a6,$6(a0) / movem.w a0-a1,$4(a6);
 *           callers VM4_Fork, VM4_SpawnChild, Wave_Start, the stage 1 effects
 */
void Obj_New(uint32_t *result, uint32_t *handler_last, uint32_t *tail_out, uint32_t *obj)
{
    if (g_ObjPoolFree-- == 0) {    /* the subq borrowed: the pool was empty */
        g_ObjPoolFree = 0;
        *result = 0xFFFFFFFFu;
        return;
    }
    ObjNode *node = dk::handle<ObjNode>((int16_t)(uint16_t)g_ObjPoolHead);
    g_ObjPoolHead = (uint16_t)(int16_t)((ObjFreeBlock *)node)->next_free;
    node->handler = *handler_last;
    ObjNode *tail = (ObjNode *)g_ObjListTail;
    ObjNode *last = dk::handle<ObjNode>(tail->prev);
    tail->prev = dk::handle_of(node);
    last->next = dk::handle_of(node);
    node->prev = dk::handle_of(last);
    node->next = dk::handle_of(tail);
    *handler_last = ADDR68(last);
    *tail_out = ADDR68(tail);
    *obj = ADDR68(node);
    *result = 0;
}

/* Obj_UnlinkFreeWithBlocks - unlinks an object from its list (prev.next = next, next.prev = prev)
 * and falls into Obj_FreeWithBlocks, which frees it and its handle blocks.
 * In:  obj (a6, linked)
 * Out: prev/next = its old neighbours (a0/a1; Obj_FreeWithBlocks then reuses them); the rest as
 *      Obj_FreeWithBlocks leaves it
 * Evidence: movem.w $4(a6),a0-a1 / move.w a1,$6(a0) / move.w a0,$4(a1), then the next file;
 *           jmp from Obj_Destroy when +4(a6) is nonzero, Stage1_SignDebrisRun's end
 */
void Obj_UnlinkFreeWithBlocks(uint32_t *d0, uint32_t *d1, uint32_t *prev, uint32_t *next, uint32_t obj)
{
    (void)d0; (void)d1;
    PTR(ObjNode) p;
    p.v = obj;
    ObjNode *node = p;
    ObjNode *before = dk::handle<ObjNode>(node->prev);
    ObjNode *after = dk::handle<ObjNode>(node->next);
    before->next = dk::handle_of(after);
    after->prev = dk::handle_of(before);
    *prev = ADDR68(before);
    *next = ADDR68(after);
    dk::jump(at_Obj_FreeWithBlocks);
}
