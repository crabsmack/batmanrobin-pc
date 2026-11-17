#include "game.h"

/* Op5_ShowGoSign - a5 op: shows the GO! sign (HUD_ShowGoSignCb), which releases the camera lock
 * when it arrives.
 * In:  a5 = script (no operands)
 * Out: d0 = HUD_ShowGoSignCb's; NEXT
 * Evidence: jsr HUD_ShowGoSignCb / NEXT; 35 uses, typically `6ED2 ... 6E9A 6A72` (all dead, GO!,
 *           end), e.g. $16B92; 17 corpus runs
 */
void Op5_ShowGoSign(uint32_t *d0, uint32_t *a5)
{
    *d0 = HUD_ShowGoSignCb();
    uint32_t a0;
    vm5_next(&a0, a5);
}

/* Op5_WaitPopulation - a5 op: waits (re-checking every frame) until g_ObjPopulation is at most n,
 * then continues.
 * In:  a5 = script: word n
 * Out: d0's low word = n; NEXT (VM5_Next) past the operand, or back to the op word and the wait
 *      tail Op5_WaitYield
 * Evidence: move.w (a5)+,d0 / cmp.w (g_ObjPopulation).w,d0 / bge VM5_Next / subq.l #4,a5 /
 *           bra Op5_WaitYield; 31 uses, e.g. $2201A (`6EA4 0009 6EEC`); 11 corpus runs
 */
void Op5_WaitPopulation(uint16_t *d0, uint32_t *a0, uint32_t *a5, uint32_t a6, dk::Flags *ccr)
{
    (void)a0, (void)a6;
    *d0 = vm5_word(a5);
    *ccr = dk::Flags::compare(*d0, g_ObjPopulation, 2, ccr->x);
    if ((int16_t)*d0 >= (int16_t)g_ObjPopulation) {
        dk::jump(at_VM5_Next);
        return;
    }
    *a5 -= 4;
    dk::jump(at_Op5_WaitYield);
}
