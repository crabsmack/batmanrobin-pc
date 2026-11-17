#include "game.h"

/* Op5_LoopA - a5 op: counts loop counter A down and branches back while it stays above 0; at 0
 * (or below) it clears the counter and continues past the offset.
 * In:  script = word offset; a6 = the wave object
 * Out: self->loop_a; the branch (Op5_Goto) or NEXT after the offset
 * Evidence: subq.w #1,$1A(a6) / bgt.w Op5_Goto / clr.w $1A(a6) / addq.l #2,a5 / NEXT; 147 script
 *           uses, e.g. $17572 (`6CB0 FFDA`); 57 corpus runs
 */
void Op5_LoopA(uint32_t *handler, const u16 **script, Vm5Wave *self)
{
    /* bgt after the subtraction: the exact difference above 0 ($8000 - 1 is not) */
    int32_t left = (int32_t)(int16_t)(uint16_t)self->loop_a - 1;
    self->loop_a = (uint16_t)left;
    if (left > 0) {
        dk::jump(at_Op5_Goto);
        return;
    }
    self->loop_a = 0;
    (*script)++;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
