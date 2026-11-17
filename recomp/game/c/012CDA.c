#include "game.h"

/* Layer_FlushTileQueue - writes every queued tile entry to the VDP and empties the queue: the
 * queue is ended at its write pointer and rewound first, then Layer_WriteTilesStart writes it
 * from the first entry. An empty queue (first count 0) writes nothing.
 * In:  ctrl/data = the VDP control and data ports (a0/a1), passed on to the writer; g_TileQueue,
 *      g_TileQueuePtr
 * Out: displacement = the first entry's count word (d0's low word); queue = g_TileQueue, then past
 *      its count (a3); old_end = the write pointer the queue was ended at (a4; not set when empty);
 *      g_TileQueuePtr = g_TileQueue, the first count cleared; then Layer_WriteTilesStart
 * Evidence: lea g_TileQueue,a3 / move.w (a3),d0 / beq.w Layer_WriteTilesEnd / clr.w (a4) / clr.w
 *           (a3)+ / bra.w Layer_WriteTilesStart; called by Layer_DrawScreen and
 *           Layer_FlushTileQueueIfIdle (the V-int); 92k TAS frames
 */
void Layer_FlushTileQueue(uint16_t *displacement, uint32_t ctrl, uint32_t data, u16 **queue, u16 **old_end)
{
    (void)ctrl;   /* the VDP ports go on to the writer in their registers */
    (void)data;
    *queue = (u16 *)g_TileQueue;
    *displacement = **queue;
    if (*displacement == 0) {
        dk::jump(at_Layer_WriteTilesEnd);
        return;
    }
    PTR(u16) end;
    end.v = (uint32_t)g_TileQueuePtr;
    *old_end = end;
    **old_end = 0;                           /* the queue ends where the writers stopped */
    g_TileQueuePtr = ADDR68(g_TileQueue);
    *(*queue)++ = 0;                         /* the next frame's queue starts empty */
    dk::jump(at_Layer_WriteTilesStart);
}
