#include "game.h"

/* DMA_ReserveSlots - reserves d0 DMA queue slots without touching the cost budget (also the second
 * half of DMA_Reserve).
 * In:  d0 = queue entries
 * Out: d0 = 0 and a0 = g_DMAQueuePtr (sign-extended: the queue is at $FFFF991C) on success; d0 = -1
 *      with the slots given back when the queue is short; the condition codes of that moveq
 * Evidence: sub.w d0,(g_DMAQueueFree).w / bmi / movea.w (g_DMAQueuePtr).w,a0 / moveq #0,d0 / rts /
 *           add.w d0,(g_DMAQueueFree).w / moveq #-1,d0; jsr $79AA with d0 = 1 from VM6_DMAStep; 8
 *           corpus runs
 */
void DMA_ReserveSlots(uint32_t *d0, uint32_t *a0)
{
    g_DMAQueueFree = (uint16_t)(g_DMAQueueFree - (uint16_t)*d0);
    if ((int16_t)g_DMAQueueFree < 0) {
        g_DMAQueueFree = (uint16_t)(g_DMAQueueFree + (uint16_t)*d0);
        *d0 = 0xFFFFFFFFu;
        return;
    }
    *a0 = (uint32_t)(int32_t)(int16_t)g_DMAQueuePtr;
    *d0 = 0;
}

/* DMA_Reserve - reserves d0 DMA queue slots and d1 units of this frame's DMA budget; the caller
 * writes its 14-byte entries at a0 and stores the pointer past them in g_DMAQueuePtr.
 * In:  d0 = queue entries, d1 = transfer cost
 * Out: d0 = 0 and a0 = g_DMAQueuePtr on success; d0 = -1 with nothing reserved when either the
 *      budget or the queue is short; the condition codes of that moveq
 * Evidence: sub.w d1,(g_DMABudgetLeft).w / bpl DMA_ReserveSlots / add.w d1,(g_DMABudgetLeft).w /
 *           moveq #-1,d0 / rts; callers Spr_MaskLeftColumn ($F808: d0 = 1, d1 = 8), $11314,
 *           $11C5E, $126D2, $144D4
 */
void DMA_Reserve(uint32_t *d0, uint16_t d1, uint32_t *a0)
{
    g_DMABudgetLeft = (uint16_t)(g_DMABudgetLeft - d1);
    if ((int16_t)g_DMABudgetLeft >= 0) {
        DMA_ReserveSlots(d0, a0);
        return;
    }
    g_DMABudgetLeft = (uint16_t)(g_DMABudgetLeft + d1);
    *d0 = 0xFFFFFFFFu;
}
