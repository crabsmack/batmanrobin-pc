#include "game.h"

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

/* DMA_FlushQueue_38 - computed entry into DMA_FlushQueue's unrolled copy: sends 38 queued DMA
 * commands (2 slots free).
 * In:  ctrl = the VDP control port (a0), a3 = the queue's start
 * Out: a3 past 9 entries, then DMA_FlushQueue_29 (the next file)
 * Evidence: $786A = $785A + 2*8: 9 x (move.l / move.l / move.l / move.w (a3)+,(a0)),
 *           then DMA_FlushQueue_29 (the next file); reached only by the jmp at $7856
 */
void DMA_FlushQueue_38(uint32_t ctrl, uint32_t *a3)
{
    for (int i = 0; i < 9; i++)
        send_entry(ctrl, a3);
    dk::jump(at_DMA_FlushQueue_29);
}

/* DMA_FlushQueue_37 - computed entry into DMA_FlushQueue's unrolled copy: sends 37 queued DMA
 * commands (3 slots free).
 * In:  ctrl = the VDP control port (a0), a3 = the queue's start
 * Out: a3 past 8 entries, then DMA_FlushQueue_29 (the next file)
 * Evidence: $7872 = $785A + 3*8: 8 x (move.l / move.l / move.l / move.w (a3)+,(a0)),
 *           then DMA_FlushQueue_29 (the next file); reached only by the jmp at $7856
 */
void DMA_FlushQueue_37(uint32_t ctrl, uint32_t *a3)
{
    for (int i = 0; i < 8; i++)
        send_entry(ctrl, a3);
    dk::jump(at_DMA_FlushQueue_29);
}

/* DMA_FlushQueue_32 - computed entry into DMA_FlushQueue's unrolled copy: sends 32 queued DMA
 * commands (8 slots free).
 * In:  ctrl = the VDP control port (a0), a3 = the queue's start
 * Out: a3 past 3 entries, then DMA_FlushQueue_29 (the next file)
 * Evidence: $789A = $785A + 8*8: 3 x (move.l / move.l / move.l / move.w (a3)+,(a0)),
 *           then DMA_FlushQueue_29 (the next file); reached only by the jmp at $7856
 */
void DMA_FlushQueue_32(uint32_t ctrl, uint32_t *a3)
{
    for (int i = 0; i < 3; i++)
        send_entry(ctrl, a3);
    dk::jump(at_DMA_FlushQueue_29);
}
