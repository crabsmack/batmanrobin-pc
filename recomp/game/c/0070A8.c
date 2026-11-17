#include "game.h"

/* The a6 VM's core ($0070A8-$0070D1): the resume hook, the reload after an op whose callee
 * clobbered a6, NEXT, the end op and the retrying native call. An a6 script is a list of op
 * handler words, each followed by its operands; every op ends by fetching the next handler word
 * (movea.w (a6)+,a0) and jumping to it, until an op parks the script (stores g_A6ScriptPc and a
 * resume hook for Frame_End) or Op6_End returns to whoever started the run. */

/* the raw code at $70D2 (incrom, never run in the corpus): Op6_CallNativeRetry's resume hook,
   which calls the routine before g_A6ScriptPc again each frame until it returns Z */
static constexpr uint32_t kVm6RetryHook = 0x70D2u;

/* VM6_Next - the a6 VM's NEXT: marks the loader script busy and jumps to the handler the next
 * script word names.
 * In:  script (a6) at a handler word; every other register as the op left it for the next one
 * Out: g_A6ScriptBusy = $FF; script past the word; the handler, jumped to
 * Evidence: st.b (g_A6ScriptBusy).w / movea.w (a6)+,a0 / jmp (a0); the tail of every plain a6 op
 *           and the return target of Op6_LoadPalettes' pea; 24 corpus runs
 */
void VM6_Next(const u16 **script)
{
    g_A6ScriptBusy = 0xFF;
    uint32_t op = dk::code((int16_t)vm6_word(script));
    dk::jump(op);
}

/* VM6_Reload - continues the script at g_A6ScriptPc: the tail of the ops whose callee clobbers a6
 * (they save the pc first), then NEXT.
 * In:  g_A6ScriptPc
 * Out: a6 = the script past the next handler word (VM6_Next), that op jumped to
 * Evidence: movea.l (g_A6ScriptPc).w,a6 falling into VM6_Next; branched to by Op6_Decompress,
 *           VM6_InitLayer, Op6_MoveLayerAToCamera, VM6_BlitMapNow, Op6_CallNativeRetry
 */
uint32_t VM6_Reload(void)
{
    PTR(const u16) pc;
    pc.v = g_A6ScriptPc;
    const u16 *script = pc;
    VM6_Next(&script);
    return ADDR68(script);
}

/* VM6_Resume - the a6 VM's resume hook: removes itself and continues the script at g_A6ScriptPc.
 * In:  g_A6ScriptPc
 * Out: g_A6ResumeHook = 0; then as VM6_Reload
 * Evidence: clr.l (g_A6ResumeHook).w falling into VM6_Reload; hook value $70A8 set by
 *           Op2_StartA6Script, VM6_Start, Op6_LoadPalettesYield, VM6_RedrawStep; the end of the
 *           wait hooks branches here; 17 corpus runs
 */
uint32_t VM6_Resume(void)
{
    g_A6ResumeHook = 0;
    return VM6_Reload();
}

/* Op6_End - a6 op: ends the loader script: clears g_A6ScriptBusy and returns to whoever ran it
 * (VM6_Run's caller, or Frame_End through the hook); with no hook left nothing resumes it.
 * In:  no operands
 * Out: g_A6ScriptBusy = 0
 * Evidence: sf.b (g_A6ScriptBusy).w / rts; word $70B8 ends 30 decoded scripts ($1FB38, $1FC4E,
 *           $1FD74); Op2_WaitA6Script* wait for the busy flag to clear; 27 corpus runs
 */
void Op6_End(void)
{
    g_A6ScriptBusy = 0;
}

/* Op6_CallNativeRetry - a6 op: calls a native routine (dc.l) and continues when it returns with Z
 * set; otherwise parks with the raw hook $70D2, which calls the routine again each frame.
 * In:  a6 = script: long routine; the registers go to the routine (a0 = its own address)
 * Out: Z: VM6_Reload (the script after the long); else g_A6ResumeHook = $70D2 and the condition
 *      codes of that move.l (all clear)
 * Evidence: movea.l (a6)+,a0 / move.l a6,(g_A6ScriptPc).w / jsr (a0) / beq VM6_Reload /
 *           move.l #$70D2,(g_A6ResumeHook).w / rts; 9 uses in the stage 2 section scripts ($33082
 *           `70BE 0003 3356`, $33092 `70BE 0003 337C`: natives ending moveq #0,d0); 4 corpus runs
 */
void Op6_CallNativeRetry(uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7,
                         uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5,
                         uint32_t *a6, dk::Flags *ccr)
{
    PTR(const u16) pc;
    pc.v = *a6;
    const u16 *script = pc;
    *a0 = vm6_long(&script);
    *a6 = ADDR68(script);
    g_A6ScriptPc = *a6;
    Op6RetryNativeFn(*a0, d0, d1, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6, ccr);
    if (ccr->z) {
        *a6 = VM6_Reload();
        return;
    }
    g_A6ResumeHook = kVm6RetryHook;
    *ccr = dk::Flags{ ccr->x, false, false, false, false };
}
