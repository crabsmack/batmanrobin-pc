#include "game.h"

/* Layer_ResetTileQueue - empties the tile queue: a 0 count ends it at the current write pointer,
 * and the write pointer goes back to the queue's start, itself ended with a 0 count.
 * In:  g_TileQueuePtr (0 at the first scene of a run)
 * Out: g_TileQueue's first count = 0, g_TileQueuePtr = g_TileQueue; the old write pointer (a4)
 * Evidence: lea g_TileQueue,a3 / movea.l (g_TileQueuePtr).w,a4 / clr.w (a4) / move.l a3,ptr /
 *           clr.w (a3); called by the scene init $9AAC, once per scene (TAS 25 times)
 */
u16 *Layer_ResetTileQueue(void)
{
    PTR(u16) old;
    old.v = (uint32_t)g_TileQueuePtr;
    if (old.raw())           /* 0 before the first scene: the 68000's clr.w hits ROM, a no-op */
        *old.get() = 0;
    g_TileQueuePtr = ADDR68(g_TileQueue);
    *(u16 *)g_TileQueue = 0;
    return old;
}
