#include "game.h"

/* Op2_SetVector0HereGotoArg / Op2_SetVector0HereGoto ($003ABE-$003ACD): a jump that leaves a
 * return point in script vector 0. */

/* Op2_SetVector0HereGotoArg - a2 op: g_VM2Arg0's high word = a word, then as
 * Op2_SetVector0HereGoto (falls into it).
 * In:  script = word arg, word offset
 * Out: g_VM2Arg0 (the word at its address, the long's high half); then Op2_SetVector0HereGoto's
 * Evidence: move.w (a2)+,(g_VM2Arg0).w, falls into $3AC2; 8 corpus runs; 25 script uses, e.g.
 *           $3932C `3ABE FD00 E340`
 */
void Op2_SetVector0HereGotoArg(const u16 **script, Vm2Obj *self)
{
    g_VM2Arg0 = ((uint32_t)*(*script)++ << 16) | (g_VM2Arg0 & 0xFFFFu);
    Op2_SetVector0HereGoto(script, self);
}

/* Op2_SetVector0HereGoto - a2 op: script vector 0 = the op right after the offset word, then the
 * script goes on at the offset's target, so Op2_GotoVector0 / Op2_GosubVector0 later come back
 * here (a jump that leaves a return point).
 * In:  script = word offset (from the word itself)
 * Out: self->vector[0] = the address after the offset; NEXT at the target
 * Evidence: movea.l a2,a0 / adda.w (a0)+,a2 / move.l a0,$32(a6) / NEXT; 115 corpus runs, 801
 *           script uses (e.g. $18246 `3AC2 0014`)
 */
void Op2_SetVector0HereGoto(const u16 **script, Vm2Obj *self)
{
    const u16 *offset = *script;
    self->vector[0] = offset + 1;
    *script = (const u16 *)((const u8 *)offset + (int16_t)(uint16_t)*offset);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
