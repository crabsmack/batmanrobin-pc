#include "game.h"

/* Obj_FreeWithBlocks ($008A48-$008ABD). */

/* the condition codes of add.b src,dst (dst + src, X = C) */
static dk::Flags add_byte_flags(uint8_t dst, uint8_t src)
{
    uint32_t sum = (uint32_t)dst + src;
    uint8_t r = (uint8_t)sum;
    bool c = sum > 0xFF;
    return dk::Flags{ c, (r & 0x80) != 0, r == 0, ((dst ^ r) & (src ^ r) & 0x80) != 0, c };
}

/* Obj_FreeWithBlocks - returns an object and every nonzero block handle in its 8 handle words to
 * the pool's free list, the object first (it does not unlink it). The blocks are chained in slot
 * order, so the last block found is the new head.
 * In:  obj (a6); d0's upper word is kept by the move.w of each handle read
 * Out: g_ObjPoolHead = the last block pushed, g_ObjPoolFree += 1 + the handles (ccr: that add.b);
 *      d0 = as the unrolled exg chain leaves it, d1 = 1 + the handles, handles = obj +$18 (a0),
 *      last = the last block pushed or obj (a1)
 * Evidence: move.w (g_ObjPoolHead).w,(a6) / movea.l a6,a1 / lea $8(a6),a0 / moveq #1,d1, then
 *           8 x (move.w (a0)+,d0 / beq / ext.l d0 / exg.l d0,a1 / move.w d0,(a1) / addq.b #1,d1),
 *           move.w a1,(g_ObjPoolHead).w / add.b d1,(g_ObjPoolFree).w; jmp from Obj_Destroy
 */
void Obj_FreeWithBlocks(uint32_t *d0, uint32_t *count, uint32_t *handles, uint32_t *last, uint32_t obj,
                        dk::Flags *ccr)
{
    PTR(ObjNode) p;
    p.v = obj;
    ObjNode *node = p;
    ((ObjFreeBlock *)node)->next_free = (int16_t)(uint16_t)g_ObjPoolHead;
    uint32_t chain = obj;          /* a1: the block pushed last */
    uint32_t n = 1;
    for (int i = 0; i < OBJ_HANDLE_SLOTS; i++) {
        uint16_t h = (uint16_t)node->blocks[i];
        *d0 = (*d0 & 0xFFFF0000u) | h;
        if (h == 0)
            continue;
        uint32_t block = (uint32_t)(int32_t)(int16_t)h;   /* ext.l */
        *d0 = chain;                                      /* exg.l d0,a1 */
        chain = block;
        dk::handle<ObjFreeBlock>((int16_t)h)->next_free = (int16_t)(uint16_t)*d0;
        n = (n & ~0xFFu) | (uint8_t)(n + 1);              /* addq.b */
    }
    g_ObjPoolHead = (uint16_t)chain;
    *ccr = add_byte_flags(g_ObjPoolFree, (uint8_t)n);
    g_ObjPoolFree = (uint8_t)(g_ObjPoolFree + (uint8_t)n);
    *count = n;
    *handles = ADDR68(&node->blocks[OBJ_HANDLE_SLOTS]);
    *last = chain;
}
