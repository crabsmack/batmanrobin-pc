#include "game.h"

/* Op2_WaitPopBelow and its coroutine forms ($00437A-$0043B5): wait until g_ObjPopulation is below
 * a limit. The plain op yields the whole object (retried next frame on this op); the coroutine
 * forms save this op in their slot and hand back to the stepping script, whose a2 the stepping op
 * pushed (Op2_StepCoroutineN). */

/* the stepping script's a2, which Op2_StepCoroutineN pushed before entering the coroutine */
static const u16 *pop_script(void)
{
    PTR(const u16) caller;
    caller.v = dk::pop_long();
    return caller;
}

/* the population is below the word operand (signed): cmp.w (a2)+,d0 / blt */
static bool population_below(uint32_t *population, const u16 **script)
{
    *population = (*population & 0xFFFF0000u) | g_ObjPopulation;
    return (int16_t)(uint16_t)*population < (int16_t)*(*script)++;
}

/* Op2_WaitPopBelow - a2 op: waits (yielding, retried every frame) until g_ObjPopulation is below
 * the operand, then goes on.
 * In:  script = word limit; g_ObjPopulation
 * Out: population (d0) low word = g_ObjPopulation; below: VM2_Next after the operand; else the
 *      script back on this op and Op2_Yield
 * Evidence: move.w (g_ObjPopulation).w,d0 / cmp.w (a2)+,d0 / blt.w VM2_Next / subq.l #4,a2 /
 *           bra.w Op2_Yield; 3 corpus runs; $18066 `437A 0001`
 */
void Op2_WaitPopBelow(uint32_t *population, uint32_t *a0, const u16 **script, uint32_t a4)
{
    (void)a0;
    (void)a4;
    if (population_below(population, script)) {
        dk::jump(at_VM2_Next);
        return;
    }
    *script -= 2;                          /* back to the op word */
    dk::jump(at_Op2_Yield);
}

/* Op2_WaitPopBelowYield0 - a2 op for coroutine 0: Op2_WaitPopBelow, waiting by saving this op in
 * slot 0 and going on in the stepping script.
 * In:  script = word limit; the stepping script's a2 on the stack
 * Out: population (d0) low word = g_ObjPopulation; below: VM2_Next; else vm->coroutine[0] = this
 *      op, the popped script, handler = NEXT's
 * Evidence: move.w (g_ObjPopulation).w,d0 / cmp.w (a2)+,d0 / blt.w VM2_Next / subq.l #4,a2 /
 *           move.l a2,$38(a4) / movea.l (a7)+,a2 / NEXT; 17 corpus runs; $36A3C `438A 000F`
 */
void Op2_WaitPopBelowYield0(uint16_t *population, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    uint32_t d0 = *population;
    bool below = population_below(&d0, script);
    *population = (uint16_t)d0;
    if (below) {
        dk::jump(at_VM2_Next);
        return;
    }
    *script -= 2;
    vm->coroutine[0] = *script;
    *script = pop_script();
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_WaitPopBelowYield1 - a2 op for coroutine 1: Op2_WaitPopBelowYield0 with slot 1.
 * In:  script = word limit; the stepping script's a2 on the stack
 * Out: as Op2_WaitPopBelowYield0, vm->coroutine[1]
 * Evidence: move.l a2,$3C(a4); 5 corpus runs; $395F4 `43A0 0004`
 */
void Op2_WaitPopBelowYield1(uint16_t *population, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    uint32_t d0 = *population;
    bool below = population_below(&d0, script);
    *population = (uint16_t)d0;
    if (below) {
        dk::jump(at_VM2_Next);
        return;
    }
    *script -= 2;
    vm->coroutine[1] = *script;
    *script = pop_script();
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
