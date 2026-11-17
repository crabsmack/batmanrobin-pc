#include "game.h"

/* ObjPool_Free, ObjPool_HasFree and ObjPool_Clear ($008764-$0087B5). */

/* ObjPool_Free - pushes the $5A-byte block back onto the object pool's free list.
 * In:  block (a0)
 * Out: g_ObjPoolHead = block, g_ObjPoolFree + 1; returns Z of that addq.b (the count wrapped to 0)
 * Evidence: move.w (g_ObjPoolHead).w,(a0) / move.w a0,(g_ObjPoolHead).w /
 *           addq.b #1,(g_ObjPoolFree).w; 13 call sites, e.g. Op4_Kill's tail $53D4, Obj_SetAttackBoxes
 */
bool ObjPool_Free(uint32_t block)
{
    PTR(ObjFreeBlock) p;
    p.v = block;
    ObjFreeBlock *b = p;
    b->next_free = (int16_t)(uint16_t)g_ObjPoolHead;
    g_ObjPoolHead = (uint16_t)block;
    return ++g_ObjPoolFree == 0;
}

/* ObjPool_HasFree - tells whether the object pool still has at least `wanted` free blocks;
 * nothing is allocated.
 * In:  wanted = blocks (d0.b)
 * Out: wanted = 0 when g_ObjPoolFree >= it, else -1 (moveq: the condition codes are its tst.l)
 * Evidence: cmp.b (g_ObjPoolFree).w,d0 / bhi / moveq #0,d0 or moveq #-1,d0; $10216 asks for 3
 *           before three ObjPool_AllocClear
 */
void ObjPool_HasFree(uint32_t *wanted)
{
    *wanted = (uint8_t)*wanted > g_ObjPoolFree ? 0xFFFFFFFFu : 0u;
}

/* ObjPool_Clear - zeroes the $5A bytes of an object block.
 * In:  block (a0)
 * Out: zero = 0 (d0); block unchanged
 * Evidence: moveq #0,d0, 22 x move.l d0,(a0)+ and a move.w, lea -$5A(a0),a0; callers $1E70A,
 *           $265DC, $370DC, $37708
 */
void ObjPool_Clear(uint32_t *zero, uint32_t *block)
{
    PTR(u8) p;
    p.v = *block;
    dk::mem_set((u8 *)p, 0, OBJ_BLOCK_SIZE);
    *zero = 0;
}
