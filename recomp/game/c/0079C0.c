#include "game.h"

/* Spr_QueueSATDMA - ends the SAT buffer's sprite link chain and queues its DMA to VRAM $0000 (one
 * queue slot, no budget); with no sprite drawn it writes one empty sprite first.
 * In:  g_SprCount, g_SATPtr (past the last entry)
 * Out: a DMA queue entry: the length (count x 4 words) in regs $93/$94, source g_SATBuffer >> 1
 *      ($95AB / $96CD / $977F), command $40000080; d1 = the length registers, a3 = the SAT
 *      pointer (+8 for the empty sprite). Nothing when the queue is full, which still counts the
 *      slot used (g_DMAQueueFree stays below 0, the original's)
 * Evidence: subq.w #1,(g_DMAQueueFree).w / bmi / movea.w (g_DMAQueuePtr).w,a0 / movea.w
 *           (g_SATPtr).w,a3 / move.w (g_SprCount).w,d0 / bne / moveq #1,d0 / clr.l (a3)+ x2 / clr.b
 *           -5(a3) / move.l #$94009300,d1 / move.b d0,d1 / add.b d1,d1 x2 / swap / lsr.w #6,d0 /
 *           move.b d0,d1 / the entry / move.w a0,(g_DMAQueuePtr).w; called once per frame by
 *           Frame_End after Spr_BuildSAT
 */
void Spr_QueueSATDMA(uint32_t *d1, uint32_t *a3)
{
    g_DMAQueueFree = (uint16_t)(g_DMAQueueFree - 1);
    if ((int16_t)g_DMAQueueFree < 0)
        return;
    PTR(u16) q;
    q.v = (uint32_t)(int32_t)(int16_t)g_DMAQueuePtr;
    *a3 = (uint32_t)(int32_t)(int16_t)g_SATPtr;
    uint16_t count = g_SprCount;
    if (count == 0) {   /* one empty sprite, so the chain has an end */
        count = 1;
        PTR(u32) sat;
        sat.v = *a3;
        *sat++ = 0;
        *sat++ = 0;
        *a3 = sat.raw();
    }
    PTR(u8) link;
    link.v = *a3 - 5;   /* the last entry's link byte */
    *link = 0;
    uint8_t len_lo = (uint8_t)((uint8_t)count * 4);   /* words = count * 4: regs $93 (low), $94 (high) */
    uint8_t len_hi = (uint8_t)(count >> 6);
    *d1 = (uint32_t)(0x9300u | len_lo) << 16 | 0x9400u | len_hi;
    *q++ = (uint16_t)(*d1 >> 16);
    *q++ = (uint16_t)*d1;
    *q++ = 0x95AB;   /* the source: g_SATBuffer ($FF9B56) >> 1 */
    *q++ = 0x96CD;
    *q++ = 0x977F;
    *q++ = 0x4000;   /* VRAM write at $0000, DMA */
    *q++ = 0x0080;
    g_DMAQueuePtr = (uint16_t)q.raw();
#if DK_MODE_NATIVE
    /* phase 6 (R10): the host table Spr_BuildSAT made goes with this table (nothing queued, as
       with a full queue: the host keeps the one it shows, as VRAM keeps its table) */
    spr_host_table_sent();
    ws_draw_sent();   /* R11: the margin pieces go with it too */
#endif
}
