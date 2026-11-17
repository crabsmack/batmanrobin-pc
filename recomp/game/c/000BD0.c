#include "game.h"

/* the stepping script's a2, which Op2_StepCoroutineN pushed before entering the coroutine */
static const u16 *pop_script(void)
{
    PTR(const u16) caller;
    caller.v = dk::pop_long();
    return caller;
}

/* The animation start / done-flag ops and coroutine 0's set-timer-and-yield ops ($000BD0-$000C2D). */

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* a coroutine's yield: the next op is where coroutine `slot` resumes; the stepping script's a2,
   pushed by Op2_StepCoroutineN, comes back off the stack and its NEXT runs (as Op2_Return) */
static void yield_coroutine(uint32_t *handler, const u16 **script, Vm2Block *vm, int slot)
{
    vm->coroutine[slot] = *script;
    *script = pop_script();
    next(handler, script);
}

/* Op2_SetAnim - a2 op: starts a sprite animation: the operand's address plus its value becomes
 * the animation script, its timer restarts and the done flag clears.
 * In:  script = a word offset (from the word itself); spr
 * Out: spr->anim, anim_timer = 0, SPR_ANIM_DONE cleared; NEXT
 * Evidence: movea.l a2,a0 / adda.w (a2)+,a0 / move.l a0,$2E(a5) / clr.w $1E(a5) / bclr #0,$18(a5);
 *           8.8k TAS frames
 */
void Op2_SetAnim(uint32_t *handler, const u16 **script, Vm2Sprite *spr)
{
    spr->anim = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    (*script)++;
    spr->anim_timer = 0;
    spr->flags &= (u8)~SPR_ANIM_DONE;
    next(handler, script);
}

/* Op2_BranchAnimFlag - a2 op: branches when the animation's done flag is set.
 * In:  script = a word offset; spr->flags
 * Out: the branch (Op2_Goto), else NEXT after the offset
 * Evidence: btst #0,$18(a5) / bne.w Op2_Goto / addq.l #2,a2 + NEXT
 */
void Op2_BranchAnimFlag(uint32_t *handler, const u16 **script, const Vm2Sprite *spr)
{
    if (spr->flags & SPR_ANIM_DONE) {
        dk::jump(at_Op2_Goto);
        return;
    }
    (*script)++;
    next(handler, script);
}

/* Op2_SetTimer0Yield0 - a2 op for coroutine 0: timer 0 = the operand, then the coroutine yields
 * (slot 0 resumes at the next op).
 * In:  script = word ticks; the stepping script's a2 on the stack
 * Out: vm->timer[0], vm->coroutine[0]; script = the popped one, NEXT
 * Evidence: move.w (a2)+,$1A(a4) / move.l a2,$38(a4) / movea.l (a7)+,a2 / NEXT
 */
void Op2_SetTimer0Yield0(uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    vm->timer[0] = *(*script)++;
    yield_coroutine(handler, script, vm, 0);
}

/* Op2_SetTimer1Yield0 - a2 op for coroutine 0: timer 1 = the operand, then the coroutine yields.
 * In:  script = word ticks; the stepping script's a2 on the stack
 * Out: vm->timer[1], vm->coroutine[0]; script = the popped one, NEXT
 * Evidence: move.w (a2)+,$1C(a4) / move.l a2,$38(a4) / movea.l (a7)+,a2 / NEXT
 */
void Op2_SetTimer1Yield0(const u16 **script, Vm2Block *vm)
{
    uint32_t handler;
    vm->timer[1] = *(*script)++;
    yield_coroutine(&handler, script, vm, 0);
}

/* Op2_SetTimer2Yield0 - a2 op for coroutine 0: timer 2 = the operand, then the coroutine yields.
 * In:  script = word ticks; the stepping script's a2 on the stack
 * Out: vm->timer[2], vm->coroutine[0]; script = the popped one, NEXT
 * Evidence: move.w (a2)+,$1E(a4) / move.l a2,$38(a4) / movea.l (a7)+,a2 / NEXT
 */
void Op2_SetTimer2Yield0(const u16 **script, Vm2Block *vm)
{
    uint32_t handler;
    vm->timer[2] = *(*script)++;
    yield_coroutine(&handler, script, vm, 0);
}
