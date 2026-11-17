#include "game.h"

/* ObjPool_AllocClear ($008718-$008763): one block off the object pool, zeroed. */

/* ObjPool_AllocClear - pops one block off the object pool and zeroes all $5A bytes of it.
 * In:  none
 * Out: block = the cleared block (sign-extended handle), unchanged when the pool is empty
 *      (g_ObjPoolFree clamps at 0); d0 (0 / -1) is clobbered: no caller reads it
 * Evidence: the ObjPool_Alloc pop, then 22 x move.l d0,(a0)+ and a move.w d0,(a0)+ /
 *           lea -$5A(a0),a0; callers $101CE (three blocks after ObjPool_HasFree(3)), $41F0E
 */
void ObjPool_AllocClear(uint32_t *block)
{
    if (g_ObjPoolFree-- == 0) {    /* the subq borrowed: the pool was empty */
        g_ObjPoolFree = 0;
        return;
    }
    ObjFreeBlock *b = dk::handle<ObjFreeBlock>((int16_t)(uint16_t)g_ObjPoolHead);
    g_ObjPoolHead = (uint16_t)(int16_t)b->next_free;
    dk::mem_set(b, 0, OBJ_BLOCK_SIZE);
    *block = ADDR68(b);
}
