#include "game.h"

/* DMA_ResetQueue - empties the VDP DMA queue and refills this frame's DMA budget.
 * In:  g_DMABudget (per scene)
 * Out: g_DMAQueuePtr = the queue's start, 40 free slots, g_DMABudgetLeft = g_DMABudget
 * Evidence: move.w #$991C,(g_DMAQueuePtr).w / move.w #$28,(g_DMAQueueFree).w / move.w
 *           (g_DMABudget).w,(g_DMABudgetLeft).w; called at every frame start ($9B86, $9BAC) and
 *           at scene init ($9AAC); 40 = the 8-byte steps of DMA_FlushQueue's unrolled copy
 */
void DMA_ResetQueue(void)
{
    g_DMAQueuePtr = (uint16_t)ADDR68(g_DMAQueue);
    g_DMAQueueFree = 40;
    g_DMABudgetLeft = g_DMABudget;
}
