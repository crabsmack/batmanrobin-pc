#include "game.h"

/* Op2_StopOsc - a2 op: stops both axes of the oscillator block in the a6 field the operand names by
 * zeroing their max speeds, so Op2_StepOsc brakes them to rest; an empty field (handle 0) does
 * nothing.
 * In:  script = word field offset (a byte offset into the object); self
 * Out: d0 = the handle; both axes' max_speed = 0; a0 = the next op's handler (NEXT)
 * Evidence: move.w (a2)+,d0 / move.w (a6,d0.w),d0 / beq VM2_Next / movea.w d0,a0 / clr.l $C(a0) /
 *           clr.l $20(a0) / NEXT; the never-run $50BA does the x axis only; 3 corpus runs; $3B37E
 *           `50A2 000E`
 */
void Op2_StopOsc(uint16_t *d0, uint32_t *a0, const u16 **script, Vm2Obj *self)
{
    uint16_t field = *(*script)++;
    *d0 = (uint16_t)*(const s16 *)((const u8 *)self + (int16_t)field);
    if (*d0 != 0) {
        Vm2Osc *osc = dk::handle<Vm2Osc>((int16_t)*d0);
        osc->x.max_speed = 0;
        osc->y.max_speed = 0;
    }
    *a0 = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*a0);
}
