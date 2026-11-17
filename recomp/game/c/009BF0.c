#include "game.h"

/* Frame_End - finishes a frame (the sync point): builds the sprite table and queues its DMA, runs the
 * loader-script hook, releases the frame to the V-int, resumes the background decompressor while
 * it waits, then the pause check, the RNG step, the scene's per-frame callback and the frame
 * counters; in slow motion it waits two more V-ints.
 * In:  g_FrameCallback, g_SlowMotion; the registers pass through to the calls
 * Out: g_FrameCounter + 1, g_FrameParity inverted, g_FrameQuarter + $40; d0 = the RNG's word, or
 *      the slow-motion wait's target; the rest as the calls hand them back. A callback that ends the
 *      scene (SceneCb_WaitFadeExit) returns past this function and its caller.
 * Evidence: jsr Spr_BuildSAT / Spr_QueueSATDMA / VM6_RunHook; clr.b g_VIntDone / g_FrameBusy /
 *           g_VIntOff; jsr BgTask_Resume; tst.b g_VIntDone / beq (wait site $9C14); jsr Pause_Update
 *           / Rand_Next / (g_FrameCallback); addq.l #1 / not.b / addi.b #$40; tst.b g_SlowMotion /
 *           cmp.l g_VIntCount,d0 / bgt (wait site $9C48); last call of all 17 scene loops
 */
void Frame_End(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
               uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a5, uint32_t a6)
{
    uint32_t a0 = 0, a1 = 0;
    uint16_t w2 = (uint16_t)*d2, w7 = (uint16_t)*d7;
    Spr_BuildSAT(d1, &w2, d3, d4, d5, d6, &w7, &a1, a2, a4, &a5, &a6);
    *d2 = (*d2 & 0xFFFF0000u) | w2;
    *d7 = (*d7 & 0xFFFF0000u) | w7;
    Spr_QueueSATDMA(d1, a3);
    VM6_RunHook(d0, d1, d2, d3, d4, d5, d6, d7, &a0, &a1, a2, a3, a4, &a5, &a6);   /* the hook runs the a6 script */
    g_VIntDone = 0;
    g_FrameBusy = 0;
    g_VIntOff = 0;
    dk::Flags flags{};
    BgTask_Resume(d0, d1, d2, d6, d7, &a0, &a1, a2, a3, a4, &flags);
    while (!g_VIntDone)
        IRQ_WAIT();
    Pause_Update(&a0, &a1);
    *d0 = (uint32_t)Rand_Next();
    FrameCallbackFn(g_FrameCallback, d0, d1, a4);
    g_FrameCounter = g_FrameCounter + 1u;
    g_FrameParity = (uint8_t)~g_FrameParity;
    g_FrameQuarter = (uint8_t)(g_FrameQuarter + 0x40);
    if (!g_SlowMotion)
        return;
    g_FrameBusy = 0xFF;
    *d0 = g_VIntCount + 2u;
    while ((int32_t)*d0 > (int32_t)(uint32_t)g_VIntCount)   /* cmp.l / bgt: signed */
        IRQ_WAIT();
    g_FrameBusy = 0;
}
