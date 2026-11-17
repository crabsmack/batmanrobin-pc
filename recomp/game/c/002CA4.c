#include "game.h"

/* Op2_LoopCounter2 and the native-call ops Op2_CallNativeRel, Op2_CallNative and
 * Op2_CallNativeRelSaved ($002CA4-$002CD5): a script calls 68000 code (a routine placed among
 * the scripts, or an absolute one) and goes on with the registers it leaves. */

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_LoopCounter2 - a2 op: decrements loop counter 2 and branches while it is still above 0.
 * In:  script = word offset; vm->counter[2]
 * Out: vm->counter[2] - 1; above 0: Op2_Goto (the branch); else NEXT after the offset
 * Evidence: subq.w #1,$18(a4) / bgt.w Op2_Goto / addq.l #2,a2 / NEXT; 15 runs
 */
void Op2_LoopCounter2(uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    /* bgt after the subtraction: the exact difference above 0 ($8000 - 1 is not) */
    int32_t left = (int32_t)(int16_t)(uint16_t)vm->counter[2] - 1;
    vm->counter[2] = (uint16_t)left;
    if (left > 0) {
        dk::jump(at_Op2_Goto);
        return;
    }
    (*script)++;
    next(handler, script);
}

/* the operand's address plus its value (movea.l a2,a0 / adda.w (a2)+,a0) */
static uint32_t relative(const u16 **script)
{
    uint32_t at = dk::addr(*script) + (uint32_t)(int32_t)(int16_t)(uint16_t)**script;
    (*script)++;
    return at;
}

/* Op2_CallNativeRel - a2 op: calls native code at a relative address (the operand's address plus
 * its value: a routine placed among the scripts) and continues the script.
 * In:  script = word offset; every register the routine reads
 * Out: every register as the routine leaves it (the script too: it may read operands); NEXT
 * Evidence: movea.l a2,a0 / adda.w (a2)+,a0 / jsr (a0) / NEXT; 131 runs; e.g. $11B82 `2CB2 014E`
 */
void Op2_CallNativeRel(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                       uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t *a3,
                       uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *a0 = relative(script);
    NativeFn(dk::code(*a0), d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, script, a3, a4, a5, a6);
    next(a0, script);
}

/* Op2_CallNative - a2 op: calls a native routine at an absolute address and continues the script:
 * the usual way scripts reach native callbacks (Fx_SpawnAt*, Spawn_At*, Pal_FlashWhite ...).
 * In:  script = long address (the routine may read more operands through the script)
 * Out: every register as the routine leaves it; its condition codes for the next op; NEXT
 * Evidence: movea.l (a2)+,a0 / jsr (a0) / NEXT; all 153 runs; $1377E `2CBC 00009584` (Pal_FlashWhite)
 */
void Op2_CallNative(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                    uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t *a3,
                    uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    const u32 *operand = (const u32 *)*script;
    *a0 = *operand;
    *script = (const u16 *)(operand + 1);
    NativeFn(dk::code(*a0), d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, script, a3, a4, a5, a6);
    next(a0, script);
}

/* Op2_CallNativeRelSaved - a2 op: as Op2_CallNativeRel, but keeps a1-a6 across the call (for
 * routines that clobber the VM's registers): they are saved on the stack around it, as the
 * 68000 did, and the script continues from the saved script pointer.
 * In:  script = word offset; every register the routine reads
 * Out: d0-d7, a0 and the condition codes as the routine leaves them; script = after the operand
 *      (as saved); NEXT
 * Evidence: movea.l a2,a0 / adda.w (a2)+,a0 / movem.l a1-a6,-(a7) / jsr (a0) /
 *           movem.l (a7)+,a1-a6 / NEXT; 24 runs; $20D32 `2CC4 FD6A`
 */
void Op2_CallNativeRelSaved(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                            uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, const u16 **script, uint32_t a3,
                            uint32_t a4, uint32_t a5, uint32_t a6)
{
    *a0 = relative(script);
    /* movem.l a1-a6,-(a7): a1 ends up at the lowest address */
    dk::push_long(a6);
    dk::push_long(a5);
    dk::push_long(a4);
    dk::push_long(a3);
    dk::push_long(dk::addr(*script));
    dk::push_long(a1);
    const u16 *routine_script = *script;
    NativeFn(dk::code(*a0), d0, d1, d2, d3, d4, d5, d6, d7, a0, &a1, &routine_script, &a3, &a4, &a5, &a6);
    /* movem.l (a7)+,a1-a6: only the script pointer is an output of the op */
    PTR(const u16) saved_script;
    dk::pop_long();
    saved_script.v = dk::pop_long();
    *script = saved_script;
    dk::pop_long();
    dk::pop_long();
    dk::pop_long();
    dk::pop_long();
    next(a0, script);
}
