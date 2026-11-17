#include "game.h"

/* Op2_StartA6Script and the waits for it ($0043E2-$00442D): an a2 script starts a loader script on
 * the global a6 VM (decompress / DMA jobs), which Frame_End resumes every frame through
 * g_A6ResumeHook (VM6_Resume) until its end op clears g_A6ScriptBusy. */

/* the stepping script's a2, which Op2_StepCoroutineN pushed before entering the coroutine */
static const u16 *pop_script(void)
{
    PTR(const u16) caller;
    caller.v = dk::pop_long();
    return caller;
}

/* Op2_StartA6Script - a2 op: starts an a6 loader script: its address in g_A6ScriptPc, the
 * per-frame resume hook = VM6_Resume, busy set.
 * In:  script = long a6 script address
 * Out: g_A6ScriptPc, g_A6ResumeHook, g_A6ScriptBusy = $FF; NEXT
 * Evidence: move.l (a2)+,(g_A6ScriptPc).w / move.l #$70A8,(g_A6ResumeHook).w /
 *           st.b (g_A6ScriptBusy).w / NEXT; $70B8 (the a6 end op) clears the flag; 7 corpus runs;
 *           $18078 `43E2 0001 FB1E 43F6` (a6 script $1FB1E)
 */
void Op2_StartA6Script(const u16 **script)
{
    uint32_t hi = *(*script)++;
    g_A6ScriptPc = (hi << 16) | *(*script)++;
    g_A6ResumeHook = at_VM6_Resume;
    g_A6ScriptBusy = 0xFF;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op2_WaitA6Script - a2 op: waits (yielding, retried every frame) until the a6 loader script has
 * finished.
 * In:  g_A6ScriptBusy
 * Out: done: VM2_Next; else the script back on this op and Op2_Yield (its d0)
 * Evidence: tst.b (g_A6ScriptBusy).w / beq.w VM2_Next / subq.l #2,a2 / bra.w Op2_Yield; 2 corpus
 *           runs; follows Op2_StartA6Script at $1807E
 */
void Op2_WaitA6Script(uint32_t *d0, uint32_t *a0, const u16 **script, uint32_t a4)
{
    (void)d0;
    (void)a0;
    (void)a4;
    if (g_A6ScriptBusy == 0) {
        dk::jump(at_VM2_Next);
        return;
    }
    (*script)--;
    dk::jump(at_Op2_Yield);
}

/* Op2_WaitA6ScriptYield0 - a2 op for coroutine 0: waits for the a6 loader script by saving this op
 * in slot 0 and going on in the stepping script.
 * In:  g_A6ScriptBusy; the stepping script's a2 on the stack
 * Out: done: VM2_Next; else vm->coroutine[0] = this op, the popped script, handler = NEXT's
 * Evidence: tst.b (g_A6ScriptBusy).w / beq.w VM2_Next / subq.l #2,a2 / move.l a2,$38(a4) /
 *           movea.l (a7)+,a2 / NEXT; 4 corpus runs; $48C1E `4404 095C`
 */
void Op2_WaitA6ScriptYield0(uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    if (g_A6ScriptBusy == 0) {
        dk::jump(at_VM2_Next);
        return;
    }
    (*script)--;
    vm->coroutine[0] = *script;
    *script = pop_script();
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_WaitA6ScriptYield1 - a2 op for coroutine 1: Op2_WaitA6ScriptYield0 with slot 1.
 * In:  g_A6ScriptBusy; the stepping script's a2 on the stack
 * Out: as Op2_WaitA6ScriptYield0, vm->coroutine[1]
 * Evidence: move.l a2,$3C(a4); 3 corpus runs; $46FB8 `4418 0BF6`
 */
void Op2_WaitA6ScriptYield1(uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    if (g_A6ScriptBusy == 0) {
        dk::jump(at_VM2_Next);
        return;
    }
    (*script)--;
    vm->coroutine[1] = *script;
    *script = pop_script();
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
