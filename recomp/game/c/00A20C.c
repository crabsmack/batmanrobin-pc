#include "game.h"

/* BgTask_Resume - if a background task is active, continues it where the V-int parked it: the
 * interrupts masked, g_BgTaskRunning set, its registers from the saved context, then a fake RTE to
 * its PC and SR (the V-int saves it again and returns to Frame_End's wait loop).
 * In:  g_BgTaskActive, g_BgTaskContext
 * Out: no task: returns (flags: g_BgTaskActive tested); else the registers d0-d2/d6-d7/a0-a4 of
 *      the context, the task running from its PC with its SR (and those condition codes)
 * Evidence: tst.b / bne / rts; ori.w #$700,sr / st.b g_BgTaskRunning / movem.l context / move.l
 *           a4,-(a7) / move.w d0,-(a7) / rte; only caller Frame_End, before it waits on g_VIntDone.
 *           A context still at Lz_Decompress's start (BgTask_Start's) is ruling 1's plain call:
 *           the same registers and SR, and Lz_Decompress's rts returns where the RTE's would
 */
void BgTask_Resume(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1,
                   uint32_t *a2, uint32_t *a3, uint32_t *a4, dk::Flags *flags)
{
    *flags = dk::Flags::test(g_BgTaskActive, 1, flags->x);
    if (!g_BgTaskActive)
        return;
    dk::set_interrupt_mask(7);
    g_BgTaskRunning = 0xFF;
    const BgTaskContext *ctx = (const BgTaskContext *)g_BgTaskContext;
    *d0 = ctx->d0;
    *d1 = ctx->d1;
    *d2 = ctx->d2;
    *d6 = ctx->d6;
    *d7 = ctx->d7;
    *a0 = ctx->a0;
    *a1 = ctx->a1;
    *a2 = ctx->a2;
    *a3 = ctx->a3;
    *a4 = ctx->a4;
    if (*a4 == at_Lz_Decompress) {
        /* ruling 1: a task not started yet (the frame-synchronous machines never park it) is a
           plain call of the decompressor from the context, with the SR the RTE would restore; its
           rts is this function's, as the RTE's PC would make it */
        dk::set_status_register((uint16_t)*d0);
        Lz_Decompress(d1, d2, d6, d7, a0, a1, a2, a3, flags);
        return;
    }
    dk::push_long(*a4);
    dk::push_word((uint16_t)*d0);
    *flags = dk::Flags::from_sr((uint16_t)*d0);
    dk::rte();
}
