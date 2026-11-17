#include "game.h"

/* The script-vector ops ($003ADE-$003B3D): Op2_SetVector1HereGoto, the vector-2 forms that also
 * set the call arguments g_VM2Arg3..0 (each falling into the next), and the gotos / gosubs through
 * vectors 0-2. The vectors are the a6 longs +$32/$36/$3A (Vm2Obj.vector). A gosub pushes the
 * script's a2 for Op2_Return ($2D2A), which pops it. */

/* a word into a call argument's high half (move.w to the long's address) */
static void set_arg_high(u32 *arg, uint16_t w) { *arg = ((uint32_t)w << 16) | ((uint32_t)*arg & 0xFFFFu); }

/* NEXT at the script: the handler its word names, jumped to */
static uint32_t jump_next(const u16 **script)
{
    uint32_t handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(handler);
    return handler;
}

/* vector = the op right after the offset word; the script goes on at the offset's target */
static void set_vector_here_goto(PTR(const u16) *vector, const u16 **script)
{
    const u16 *offset = *script;
    *vector = offset + 1;
    *script = (const u16 *)((const u8 *)offset + (int16_t)(uint16_t)*offset);
}

/* Op2_SetVector1HereGoto - a2 op: script vector 1 = the op right after the offset word, then the
 * script goes on at the offset's target.
 * In:  script = word offset (from the word itself)
 * Out: self->vector[1] = the address after the offset; NEXT at the target
 * Evidence: movea.l a2,a0 / adda.w (a0)+,a2 / move.l a0,$36(a6) / NEXT; 105 corpus runs, 376
 *           script uses (e.g. $18264 `3ADE 14F6`)
 */
void Op2_SetVector1HereGoto(const u16 **script, Vm2Obj *self)
{
    set_vector_here_goto(&self->vector[1], script);
    jump_next(script);
}

/* Op2_SetVector2HereGotoArgs4 - a2 op: g_VM2Arg3 and g_VM2Arg2 = words, then the Args2 form
 * (falls into it).
 * In:  script = words arg3, arg2, arg1, arg0, offset
 * Out: the four arguments' high halves; self->vector[2]; NEXT at the target (handler = its code)
 * Evidence: move.w (a2)+,(g_VM2Arg3).w / move.w (a2)+,(g_VM2Arg2).w, falls into $3AF2; 33 corpus
 *           runs; $2A030 `3AEA 0014 FFEC 00D2 00B4 DCFA`
 */
void Op2_SetVector2HereGotoArgs4(uint32_t *handler, const u16 **script, Vm2Obj *self)
{
    set_arg_high(&g_VM2Arg3, *(*script)++);
    set_arg_high(&g_VM2Arg2, *(*script)++);
    Op2_SetVector2HereGotoArgs2(handler, script, self);
}

/* Op2_SetVector2HereGotoArgs2 - a2 op: g_VM2Arg1 = a word, then the Arg form (falls into it).
 * In:  script = words arg1, arg0, offset
 * Out: as Op2_SetVector2HereGotoArgs4
 * Evidence: move.w (a2)+,(g_VM2Arg1).w; dispatched at $3AF2 (37 corpus runs; e.g. $288DC)
 */
void Op2_SetVector2HereGotoArgs2(uint32_t *handler, const u16 **script, Vm2Obj *self)
{
    set_arg_high(&g_VM2Arg1, *(*script)++);
    Op2_SetVector2HereGotoArg(handler, script, self);
}

/* Op2_SetVector2HereGotoArg - a2 op: g_VM2Arg0 = a word, then Op2_SetVector2HereGoto (falls into
 * it).
 * In:  script = words arg0, offset
 * Out: as Op2_SetVector2HereGotoArgs4
 * Evidence: move.w (a2)+,(g_VM2Arg0).w; dispatched at $3AF6 (3 corpus runs); $18580 `3AF6 000C 1276`
 */
void Op2_SetVector2HereGotoArg(uint32_t *handler, const u16 **script, Vm2Obj *self)
{
    set_arg_high(&g_VM2Arg0, *(*script)++);
    Op2_SetVector2HereGoto(handler, script, self);
}

/* Op2_SetVector2HereGoto - a2 op: script vector 2 = the op right after the offset word, then the
 * script goes on at the offset's target (no arguments).
 * In:  script = word offset (from the word itself)
 * Out: self->vector[2]; NEXT at the target (handler = its code)
 * Evidence: movea.l a2,a0 / adda.w (a0)+,a2 / move.l a0,$3A(a6) / NEXT; 101 corpus runs, 665
 *           script uses (e.g. $18530 `3AFA 1254`)
 */
void Op2_SetVector2HereGoto(uint32_t *handler, const u16 **script, Vm2Obj *self)
{
    set_vector_here_goto(&self->vector[2], script);
    *handler = jump_next(script);
}

/* Op2_GotoVector0 - a2 op: the script goes on at script vector 0.
 * In:  self->vector[0] (Op2_SetVector0 / Op2_SetVector0HereGoto)
 * Out: the script (a2) past the handler word; NEXT at the vector
 * Evidence: movea.l $32(a6),a2 / NEXT; 83 corpus runs; $1823C `3B06 3B0E 3B16`
 */
uint32_t Op2_GotoVector0(const Vm2Obj *self)
{
    const u16 *script = self->vector[0];
    jump_next(&script);
    return ADDR68(script);
}

/* Op2_GotoVector1 - a2 op: the script goes on at script vector 1.
 * In:  self->vector[1] (Op2_SetVector1 / Op2_SetVector1HereGoto)
 * Out: the script (a2) past the handler word; NEXT at the vector
 * Evidence: movea.l $36(a6),a2 / NEXT; 75 corpus runs; $198C8 `3B0E 433E 0017`
 */
uint32_t Op2_GotoVector1(const Vm2Obj *self)
{
    const u16 *script = self->vector[1];
    jump_next(&script);
    return ADDR68(script);
}

/* Op2_GotoVector2 - a2 op: the script goes on at script vector 2.
 * In:  self->vector[2] (the Op2_SetVector2HereGoto family)
 * Out: the script (a2) past the handler word; NEXT at the vector
 * Evidence: movea.l $3A(a6),a2 / NEXT; 92 corpus runs; $1981C `3B16`
 */
uint32_t Op2_GotoVector2(const Vm2Obj *self)
{
    const u16 *script = self->vector[2];
    jump_next(&script);
    return ADDR68(script);
}

/* Op2_GosubVector0 - a2 op: calls the script at vector 0 as a subroutine: the next op's address
 * pushed for Op2_Return.
 * In:  script = the next op (no operand); self->vector[0]
 * Out: (sp) = the return script; the script past the vector's handler word; NEXT at the vector
 * Evidence: pea.l (a2) / movea.l $32(a6),a2 / NEXT; Op2_Return ($2D2A) pops a2; 42 corpus runs;
 *           $27CA8 `3B1E 1008 0004 3B28 2D2A`
 */
void Op2_GosubVector0(const u16 **script, Vm2Obj *self)
{
    dk::push_long(ADDR68(*script));
    *script = self->vector[0];
    jump_next(script);
}

/* Op2_GosubVector1 - a2 op: calls the script at vector 1 as a subroutine (return pushed).
 * In:  script = the next op (no operand); self->vector[1]
 * Out: (sp) = the return script; NEXT at the vector
 * Evidence: pea.l (a2) / movea.l $36(a6),a2 / NEXT; 40 corpus runs; $13FE6 `3B28 2D22 FFDA`
 */
void Op2_GosubVector1(const u16 **script, Vm2Obj *self)
{
    dk::push_long(ADDR68(*script));
    *script = self->vector[1];
    jump_next(script);
}

/* Op2_GosubVector2 - a2 op: calls the script at vector 2 as a subroutine (return pushed).
 * In:  script = the next op (no operand); self->vector[2]
 * Out: (sp) = the return script; NEXT at the vector
 * Evidence: pea.l (a2) / movea.l $3A(a6),a2 / NEXT; 15 corpus runs; $286CC `3B32 3EB8 0574`
 */
void Op2_GosubVector2(const u16 **script, Vm2Obj *self)
{
    dk::push_long(ADDR68(*script));
    *script = self->vector[2];
    jump_next(script);
}
