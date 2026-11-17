#include "game.h"

/* DMA_FlushQueue's unrolled copy ($00785A-$00799B): one 8-byte step per queued DMA command; its
 * computed jump enters at $785A + free slots x 8, so the entries left of the end send the
 * queued commands. DMA_FlushQueue's C sends them itself, so these run only if something enters
 * them; V-int code: D2 proves them. */

/* one queued DMA command to the VDP control port: VDP regs $93/$94, $95/$96, $97, the command
   (move.l / move.l / move.l / move.w (a3)+,(a0)); the last word written, for the condition codes */
static uint16_t send_entry(uint32_t ctrl, uint32_t *a3)
{
    PTR(const u16) q;
    q.v = *a3;
    for (int i = 0; i < 3; i++) {
        uint32_t hi = (uint16_t)*q++;
        hw::port_write32(ctrl, hi << 16 | (uint16_t)*q++);
    }
    uint16_t last = *q++;
    hw::port_write16(ctrl, last);
    *a3 = q.raw();
    return last;
}

/* DMA_FlushQueue_40 - computed entry into DMA_FlushQueue's unrolled copy: sends 40 queued DMA
 * commands (0 slots free).
 * In:  ctrl = the VDP control port (a0), a3 = the queue's start
 * Out: a3 past 2 entries, then DMA_FlushQueue_38 (the next file)
 * Evidence: $785A = $785A + 0*8: 2 x (move.l / move.l / move.l / move.w (a3)+,(a0)),
 *           then DMA_FlushQueue_38 (the next file); reached only by the jmp at $7856
 */
void DMA_FlushQueue_40(uint32_t ctrl, uint32_t *a3)
{
    for (int i = 0; i < 2; i++)
        send_entry(ctrl, a3);
    dk::jump(at_DMA_FlushQueue_38);
}
