#include "game.h"

/* ObjPool_Init ($0086E6-$0086FD): builds the object pool's free list at scene init. */

/* ObjPool_Init - links `count` blocks of $5A bytes from `pool` on into the object pool's free
 * list: each block's first word is the next block's address (the last block's is left as it was).
 * In:  count = blocks (d0.w; dbra counter); pool = the first block (a0)
 * Out: g_ObjPoolFree = count's low byte, g_ObjPoolHead = pool's low word
 * Evidence: move.b d0,(g_ObjPoolFree).w / move.w a0,(g_ObjPoolHead).w / the lea $5A(a0),a0 /
 *           move.w a0,-$5A(a0) dbra loop; only caller the scene init $9AAC (a0 = g_ObjPool, d0 = 140)
 */
void ObjPool_Init(uint16_t count, uint32_t pool)
{
    g_ObjPoolFree = (uint8_t)count;
    g_ObjPoolHead = (uint16_t)pool;
    PTR(ObjFreeBlock) first;
    first.v = pool;
    ObjFreeBlock *block = first;
    for (uint16_t n = count; n-- != 0; block++)   /* dbra: count links */
        block->next_free = dk::handle_of(block + 1);
}
