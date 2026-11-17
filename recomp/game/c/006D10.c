#include "game.h"

/* $006D10-$006D59: the a5 script flow: loop B, a native call, gosub / return on the wave
 * director's own return stack (a word pointer at +$34, the stack from +$36), a random switch and
 * the relative goto they branch through. */

static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op5_LoopB - a5 op: counts loop counter B down and branches back while it stays above 0; at 0 it
 * clears the counter and continues past the offset (the outer / inner twin of Op5_LoopA).
 * In:  script = word offset; a6 = the wave object
 * Out: self->loop_b; the branch (Op5_Goto) or NEXT after the offset
 * Evidence: subq.w #1,$1C(a6) / bgt.w Op5_Goto / clr.w $1C(a6) / addq.l #2,a5 / NEXT; script uses
 *           $21E92 (`6D10 FFF0`), $21F84; 16 corpus runs
 */
void Op5_LoopB(uint32_t *handler, const u16 **script, Vm5Wave *self)
{
    int32_t left = (int32_t)(int16_t)(uint16_t)self->loop_b - 1;   /* bgt: the exact difference */
    self->loop_b = (uint16_t)left;
    if (left > 0) {
        Op5_Goto(handler, script);
        return;
    }
    self->loop_b = 0;
    (*script)++;
    next(handler, script);
}

/* Op5_CallNative - a5 op: calls the 68000 routine at the absolute address in the operand and
 * continues with the next op.
 * In:  script = long routine address; every register the routine reads
 * Out: every register as the routine leaves it; NEXT
 * Evidence: movea.l (a5)+,a0 / jsr (a0) / NEXT; script uses $21752 (`6D22 0002 40EE`), $21788
 *           (`0002 40F6`); 12 corpus runs. Prototype: every register (was d0 / a0 / a5): the routine
 *           is called through the Op5NativeFn type
 */
void Op5_CallNative(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                    uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4,
                    const u16 **script, uint32_t *a6)
{
    *a0 = (uint32_t)(uint16_t)(*script)[0] << 16 | (uint16_t)(*script)[1];
    *script += 2;
    Op5NativeFn(dk::code(*a0), d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, script, a6);
    next(a0, script);
}

/* Op5_Gosub - a5 op: calls a wave-script subroutine at an offset: pushes the script position (the
 * offset word's address) onto the object's own return stack and branches.
 * In:  script = word offset; a6 = the wave object (rstack)
 * Out: a0 = the handler; self->rstack + 4; the branch
 * Evidence: movea.w $34(a6),a0 / move.l a5,(a0)+ / move.w a0,$34(a6) / adda.w (a5),a5 / NEXT;
 *           Wave_Start sets $34 = a6 + $36; 103 script uses, e.g. $21D62 (`6D2A 1F38`); 43 corpus runs
 */
void Op5_Gosub(uint32_t *handler, const u16 **script, Vm5Wave *self)
{
    PTR(const u16) *top = dk::handle<PTR(const u16)>((int16_t)(uint16_t)self->rstack);
    *top = *script;
    self->rstack = (uint16_t)(self->rstack + 4);
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    next(handler, script);
}

/* Op5_Return - a5 op: returns from an Op5_Gosub: pops the script position from the return stack
 * and continues after the call's offset word.
 * In:  script (no operands); a6 = the wave object (rstack)
 * Out: self->rstack - 4; script = the popped position + 2; NEXT
 * Evidence: movea.w $34(a6),a0 / movea.l -(a0),a5 / move.w a0,$34(a6) / addq.l #2,a5 / NEXT; 150
 *           script uses, e.g. $21E6A, $21EA4; 42 corpus runs
 */
void Op5_Return(uint32_t *handler, const u16 **script, Vm5Wave *self)
{
    self->rstack = (uint16_t)(self->rstack - 4);
    *script = *dk::handle<PTR(const u16)>((int16_t)(uint16_t)self->rstack);
    (*script)++;
    next(handler, script);
}

/* Op5_RandomSwitch - a5 op: branches to one of n offsets picked at random (Rand_Below(n)).
 * In:  script = word n, n word offsets (each relative to itself)
 * Out: d0's low word = the doubled pick; then Op5_Goto at the picked offset
 * Evidence: move.w (a5)+,d0 / bsr.w Rand_Below / add.w d0,d0 / adda.w d0,a5, falling into Op5_Goto;
 *           60 script uses, e.g. $16DA2 (`6D4A 0002 FFB8 FFD8`), $17640; 48 corpus runs
 */
void Op5_RandomSwitch(uint32_t *d0, uint32_t *handler, const u16 **script)
{
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)*(*script)++;
    Rand_Below(d0);
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)(*d0 * 2);
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)*d0);
    Op5_Goto(handler, script);
}

/* Op5_Goto - a5 op: relative goto (the offset word's address plus its value); also the taken path
 * of Op5_LoopA / LoopB and Op5_RandomSwitch.
 * In:  script = word offset
 * Out: the branch; NEXT
 * Evidence: adda.w (a5),a5 / NEXT; 149 script uses, e.g. $16992 (`6D54 FFAE`); 52 corpus runs
 */
void Op5_Goto(uint32_t *handler, const u16 **script)
{
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    next(handler, script);
}
