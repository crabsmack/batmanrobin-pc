#include "game.h"

/* ObjPool_Alloc ($0086FE-$008717): one block off the object pool, uncleared. */

/* ObjPool_Alloc - pops one $5A-byte block off the object pool's free list without clearing it.
 * In:  none
 * Out: result = 0 and block = the block (sign-extended handle, as movea.w), or result = -1 with
 *      block unchanged when the pool is empty (g_ObjPoolFree clamps at 0); the condition codes
 *      are tst.l of result (moveq)
 * Evidence: subq.b #1,(g_ObjPoolFree).w / bcs / movea.w (g_ObjPoolHead).w,a0 /
 *           move.w (a0),(g_ObjPoolHead).w / moveq #0,d0, or clr.b / moveq #-1,d0; callers $120E,
 *           Obj_SetAttackBoxes, $385A, $3B76, $4F4E, $66E2, $6EBA, $37260
 */
void ObjPool_Alloc(uint32_t *result, uint32_t *block)
{
    if (g_ObjPoolFree-- == 0) {    /* the subq borrowed: the pool was empty */
        g_ObjPoolFree = 0;
        *result = 0xFFFFFFFFu;
        return;
    }
    ObjFreeBlock *b = dk::handle<ObjFreeBlock>((int16_t)(uint16_t)g_ObjPoolHead);
    g_ObjPoolHead = (uint16_t)(int16_t)b->next_free;
    *block = ADDR68(b);
    *result = 0;
}
