#include "game.h"

/* DMA_FlushQueue - writes every queued 14-byte DMA command (VDP regs $93-$97, then the command's
 * two words) to the VDP control port: 40 - g_DMAQueueFree of them from the queue's start. The
 * original jumps into an unrolled copy at $785A + free x 8 (DMA_FlushQueue_40 ...); the C sends
 * the same words in the same order itself. The queue itself is not reset (DMA_ResetQueue).
 * In:  ctrl = the VDP control port (a0); g_DMAQueueFree, g_DMAQueue
 * Out: the commands written (each starts its DMA)
 * Evidence: lea g_DMAQueue,a3 / move.w (g_DMAQueueFree).w,d0 / lsl.w #3,d0 / jmp $785A(pc,d0.w)
 *           into move.l / move.l / move.l / move.w (a3)+,(a0) steps ending in the rts at $799A; only
 *           caller VInt_Handler (V-int code: D2 proves it). A free count outside 0-40 (only after a
 *           full queue's Spr_QueueSATDMA leaves it at -1) jumps outside the copy in the original;
 *           the C then sends nothing
 */
void DMA_FlushQueue(uint32_t ctrl)
{
    int16_t free_slots = (int16_t)g_DMAQueueFree;
    if (free_slots < 0 || free_slots > 40)
        return;
    PTR(const u16) q;
    q.v = ADDR68(g_DMAQueue);
    for (int n = 40 - free_slots; n > 0; n--) {
        for (int i = 0; i < 3; i++) {
            uint32_t hi = (uint16_t)*q++;
            hw::port_write32(ctrl, hi << 16 | (uint16_t)*q++);
        }
        hw::port_write16(ctrl, *q++);
    }
}
