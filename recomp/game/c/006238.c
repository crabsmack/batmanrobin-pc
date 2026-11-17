#include "game.h"

/* Op4_SpawnAtDrawPos / Op4_CalcDrawPos ($006238-$00624B): the object's drawn point
 * (VM4_CalcDrawPos) as the next spawn's position, or as pixel words like the motion ops leave. */

/* Op4_SpawnAtDrawPos - a4 op: sets the spawn position registers d4 / d5 to the object's drawn
 * point (16.16), so the next spawn starts where the object is drawn.
 * In:  script (no operands); a6 = the object
 * Out: d4 / d5 = the drawn x / y (16.16); d0 / d1 as VM4_CalcDrawPos leaves them; NEXT
 * Evidence: bsr.w VM4_CalcDrawPos / NEXT; script uses $14C3C (`6238 557A ...` then
 *           Op4_ForkRestAndEnd); 146 corpus runs
 */
void Op4_SpawnAtDrawPos(uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, const u16 **script, const Vm4Obj *self)
{
    VM4_CalcDrawPos(d0, d1, d4, d5, self);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op4_CalcDrawPos - a4 op: computes the object's drawn point without moving it and leaves its
 * pixel words in the low words of d4 / d5 (as the motion ops do).
 * In:  script (no operands); a6 = the object
 * Out: d4 / d5 = the drawn point swapped (pixel word low, fraction high); d0 / d1 as
 *      VM4_CalcDrawPos leaves them; NEXT
 * Evidence: bsr.w VM4_CalcDrawPos / swap d4 / swap d5 / NEXT; script use $302A2 (`6240 67D2`); 9
 *           corpus runs
 */
void Op4_CalcDrawPos(uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, const u16 **script, const Vm4Obj *self)
{
    VM4_CalcDrawPos(d0, d1, d4, d5, self);
    *d4 = *d4 << 16 | *d4 >> 16;
    *d5 = *d5 << 16 | *d5 >> 16;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
