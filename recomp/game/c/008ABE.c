#include "game.h"

/* Obj_UnlinkFree ($008ABE-$008AD9). */

/* Obj_UnlinkFree - unlinks a one-block object from its list and pushes the block back on the
 * object pool (the inverse of Obj_New).
 * In:  obj (a6)
 * Out: prev/next = its old neighbours (a0/a1, sign-extended handles); g_ObjPoolHead = obj,
 *      g_ObjPoolFree + 1, ccr = that addq.b's condition codes
 * Evidence: movem.w $4(a6),a0-a1 / move.w a1,$6(a0) / move.w a0,$4(a1) /
 *           move.w (g_ObjPoolHead).w,(a6) / move.w a6,(g_ObjPoolHead).w /
 *           addq.b #1,(g_ObjPoolFree).w; Op4_Kill's tail, Op4_ShotStep, VM5_EndWave
 */
void Obj_UnlinkFree(uint32_t *prev, uint32_t *next, uint32_t obj, dk::Flags *ccr)
{
    PTR(ObjNode) p;
    p.v = obj;
    ObjNode *node = p;
    ObjNode *before = dk::handle<ObjNode>(node->prev);
    ObjNode *after = dk::handle<ObjNode>(node->next);
    before->next = dk::handle_of(after);
    after->prev = dk::handle_of(before);
    ((ObjFreeBlock *)node)->next_free = (int16_t)(uint16_t)g_ObjPoolHead;
    g_ObjPoolHead = (uint16_t)obj;
    uint8_t was = g_ObjPoolFree;
    uint8_t now = (uint8_t)(was + 1);
    g_ObjPoolFree = now;
    bool c = was == 0xFF;
    *ccr = dk::Flags{ c, (now & 0x80) != 0, now == 0, was == 0x7F, c };
    *prev = ADDR68(before);
    *next = ADDR68(after);
}
