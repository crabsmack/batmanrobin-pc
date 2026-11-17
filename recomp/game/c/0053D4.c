#include "game.h"

/* The a4 VM's flow ops ($0053D4-$00544F): resume points, the yields and returns, the script
 * subroutine call, killing the object, the gotos and the native call. An op ends with NEXT (the
 * next word names the next op's handler), a branch, or an rts that ends the run. */

/* NEXT: the handler the script's next word names, jumped to */
static void op_next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* the script address an offset word names: the word's own address plus its value */
static const u16 *rel_target(const u16 *word)
{
    return (const u16 *)((const u8 *)word + (int16_t)(uint16_t)*word);
}

/* the handler word at a script address (a register holding it), and the address past it */
static uint32_t fetch_at(uint32_t *a4)
{
    PTR(const u16) script;
    script.v = *a4;
    uint32_t handler = dk::code((int16_t)(uint16_t)*script++);
    *a4 = script.raw();
    return handler;
}

/* Op4_SetResume - a4 op: the object's resume pc becomes the operand's address plus its value;
 * the script continues here.
 * In:  script = a word offset (from the word itself)
 * Out: self->resume; NEXT after the operand
 * Evidence: movea.l a4,a0 / adda.w (a4)+,a0 / move.l a0,$8(a6); script uses $1A3B6, $1A5CC, $1F234
 *           (an offset back to a draw loop); 39 corpus runs
 */
void Op4_SetResume(const u16 **script, Vm4Obj *self)
{
    self->resume = rel_target(*script);
    (*script)++;
    uint32_t handler;
    op_next(&handler, script);
}

/* Op4_SetResumeHere - a4 op: the next op becomes the resume pc, so later frames restart here.
 * In:  script = the next op (no operand)
 * Out: self->resume; NEXT
 * Evidence: move.l a4,$8(a6) then NEXT; script uses $15062, $150A0 (followed by the motion op
 *           $6286); 61 corpus runs
 */
void Op4_SetResumeHere(const u16 **script, Vm4Obj *self)
{
    self->resume = *script;
    uint32_t handler;
    op_next(&handler, script);
}

/* Op4_YieldGotoAbs - a4 op: ends this frame's run; the object resumes next frame at the absolute
 * address in the operand.
 * In:  script = a long script address
 * Out: script past it; self->resume; returns to the runner (or the spawn script's native caller)
 *      with the condition codes of the move.l
 * Evidence: move.l (a4)+,$8(a6) / rts; script uses $1AB70, $25B82 (dc.l $000151B2 / $0001513E);
 *           56 corpus runs
 */
void Op4_YieldGotoAbs(const u16 **script, Vm4Obj *self, dk::Flags *ccr)
{
    PTR(const u32) operand;
    operand.v = dk::addr(*script);
    uint32_t target = *operand;
    *script += 2;
    self->resume.v = target;
    *ccr = dk::Flags::test(target, 4);
}

/* Op4_YieldGoto - a4 op: ends this frame's run; the object resumes next frame at the operand's
 * address plus its value.
 * In:  script = a word offset
 * Out: script = the target; self->resume = it; returns (Op4_Return) with the move.l's condition
 *      codes
 * Evidence: adda.w (a4),a4 / move.l a4,$8(a6), falling into Op4_Return's rts; script uses $26BA0,
 *           $26D0E, $363DC; 37 corpus runs
 */
void Op4_YieldGoto(const u16 **script, Vm4Obj *self, dk::Flags *ccr)
{
    *script = rel_target(*script);
    self->resume = *script;
    *ccr = dk::Flags::test(dk::addr(*script), 4);
    Op4_Return();
}

/* Op4_Return - a4 op: a plain rts: returns from an Op4_Gosub subroutine, or ends the run of a
 * spawn script / this frame's run of an object without moving its resume pc.
 * In:  none
 * Out: returns to Op4_Gosub, Fx_RunScript or the object runner
 * Evidence: the rts of Op4_YieldGoto, a label of its own; 188 a4-script uses, e.g. the shot scripts
 *           $108D6/$108FC end with it after their spawn op; 151 corpus runs
 */
void Op4_Return(void)
{
}

/* Op4_Gosub - a4 op: calls a script subroutine at the operand's address plus its value: the
 * script after the operand is pushed, the subroutine's ops run until one returns, then the pushed
 * script is popped and continues.
 * In:  a4 = script: a word offset; every register (the subroutine's ops read and leave any)
 * Out: what the subroutine left; a4 = the popped script past its next handler word; that op,
 *      jumped to
 * Evidence: pea $2(a4) / adda.w (a4),a4 / movea.w (a4)+,a0 / jsr (a0) / movea.l (a7)+,a4 / NEXT;
 *           script uses $1AAA6, $1ED1A, $26B0A; 11 corpus runs
 */
void Op4_Gosub(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
               uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5,
               uint32_t *a6)
{
    PTR(const u16) offset;
    offset.v = *a4;
    dk::push_long(*a4 + 2);
    *a4 = dk::addr(rel_target(offset));
    *a0 = fetch_at(a4);
    Op4CodeFn(*a0, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
    *a4 = dk::pop_long();
    *a0 = fetch_at(a4);
    dk::jump(*a0);
}

/* Op4_Kill - a4 op: deletes the object: frees its collision box (unlinked from its list) if it
 * has one, releases its sprite frame's art, then unlinks it from the object list and frees its
 * pool block (Obj_UnlinkFree, whose rts ends the run).
 * In:  self = the object; a1 = Hitbox_Unlink's list input as the op was entered
 * Out: d0 = the box handle; a0 = the box, then what Spr_ReleaseFrame leaves; d2 = the frame shown;
 *      jumped to Obj_UnlinkFree
 * Evidence: move.w $14(a6),d0 / beq / movea.w d0,a0 / bsr Hitbox_Unlink / bsr ObjPool_Free / move.w
 *           $20(a6),d2 / bsr Spr_ReleaseFrame / bra Obj_UnlinkFree; 63 a4-script uses ($108F2 shot
 *           scripts); also the target of the off-screen and counter ops; 151 corpus runs
 */
void Op4_Kill(uint16_t *d0, uint16_t *d2, uint32_t *a0, uint32_t *a1, Vm4Obj *self)
{
    *d0 = (uint16_t)self->box;
    if (*d0 != 0) {
        *a0 = (uint32_t)(int32_t)(int16_t)*d0;
        Hitbox_Unlink(*a0, *a1);
        ObjPool_Free(*a0);
    }
    *d2 = self->frame_shown;
    Spr_ReleaseFrame(d0, d2, a0);
    dk::jump(at_Obj_UnlinkFree);
}

/* Op4_GotoAbs - a4 op: continues at the absolute script address in the operand.
 * In:  script = a long script address
 * Out: script = the target; NEXT there
 * Evidence: movea.l (a4),a4 then NEXT; script uses $1ABB2, $1F11E (dc.l $00014C58 / $00014C3C);
 *           28 corpus runs
 */
void Op4_GotoAbs(const u16 **script)
{
    PTR(const u16) target;
    target.v = *(const u32 *)*script;
    *script = target;
    uint32_t handler;
    op_next(&handler, script);
}

/* Op4_RandomSwitch - a4 op: branches to one of n relative offsets picked at random.
 * In:  script = word n, then n word offsets (each from its own address)
 * Out: d0 = twice the pick (low word; Rand_Below's result); then Op4_Goto at the picked offset
 * Evidence: move.w (a4)+,d0 / bsr Rand_Below / add.w d0,d0 / adda.w d0,a4, falling into Op4_Goto;
 *           script uses $20EC4 (n = 4), $278EA (n = 6), $30652 (n = 3); 54 corpus runs
 */
void Op4_RandomSwitch(uint32_t *d0, uint32_t *handler, const u16 **script)
{
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)*(*script)++;
    Rand_Below(d0);
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)(*d0 + *d0);
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)*d0);
    Op4_Goto(handler, script);
}

/* Op4_Goto - a4 op: relative goto: continues at the operand's address plus its value; also the
 * taken path of the a4 branch ops.
 * In:  script = a word offset
 * Out: script = the target; NEXT there
 * Evidence: adda.w (a4),a4 then NEXT; 130 a4-script uses (e.g. $132BA, $146CC); the branch target
 *           of Op4_TimerBranchOut, Op4_CounterBranchOut, Op4_TickerLoop, the box op $66E2
 */
void Op4_Goto(uint32_t *handler, const u16 **script)
{
    *script = rel_target(*script);
    op_next(handler, script);
}

/* Op4_CallNative - a4 op: calls the 68000 routine at the absolute address in the operand (it sees
 * the script's registers and may move a4, e.g. to a branch target) and continues with the next op.
 * In:  a4 = script: a long routine address; every register (the routine reads and leaves any)
 * Out: what the routine left; a4 past the next handler word; that op, jumped to
 * Evidence: movea.l (a4)+,a0 / jsr (a0) / NEXT; 76 a4-script uses: Arc_PlanToMark $926C, Arc_Step
 *           $92EA, Shot_DamageTarget (moves a4 to a branch target), $13296; 148 corpus runs
 */
void Op4_CallNative(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                    uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                    uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    PTR(const u32) operand;
    operand.v = *a4;
    *a0 = *operand;
    *a4 += 4;
    Op4CodeFn(*a0, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
    *a0 = fetch_at(a4);
    dk::jump(*a0);
}
