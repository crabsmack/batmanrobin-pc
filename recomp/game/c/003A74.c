#include "game.h"

/* Op2_SwitchAttacker / Op2_SetVector0 / Op2_SetVector1 ($003A74-$003AA5): a branch on who struck
 * the object, and the two ops that point a script vector at a relative address. */

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

/* Op2_SwitchAttacker - a2 op: branches by who struck the object: the attacker id of g_StruckBox
 * compared with a list of (id, offset) pairs, the first match taken (Op2_Goto).
 * In:  script = word count-1, then pairs (word id, word offset); g_StruckBox
 * Out: box = g_StruckBox; attacker = its struck_by; count = the dbra counter (its last value:
 *      $FFFF when nothing matched); a match goes on at Op2_Goto with the script at its offset word,
 *      else NEXT after the list
 * Evidence: movea.l (g_StruckBox).w,a0 / move.w $1A(a0),d0 / move.w (a2)+,d7 / cmp.w (a2)+,d0 /
 *           beq.w Op2_Goto / addq.l #2,a2 / dbra d7; 33 runs; $1AFB6 `3A74 0001 0001 0036 0002 0032`
 */
void Op2_SwitchAttacker(uint16_t *attacker, uint16_t *count, uint32_t *box_or_handler, const u16 **script)
{
    PTR(const HitBox) box;
    box.v = g_StruckBox;
    *box_or_handler = box.raw();
    *attacker = box->struck_by;
    *count = *(*script)++;
    for (;;) {
        if (*(*script)++ == *attacker) {
            dk::jump(at_Op2_Goto);
            return;
        }
        (*script)++;                       /* this pair's offset */
        *count = (uint16_t)(*count - 1);
        if (*count == 0xFFFF)
            break;
    }
    op_next(box_or_handler, script);
}

/* Op2_SetVector0 - a2 op: script vector 0 = a relative script address (the target of
 * Op2_GotoVector0 / Op2_GosubVector0 and Op2_TickTimer0GotoV0).
 * In:  script = word offset (from the word itself)
 * Out: self->vector[0]; NEXT after the operand
 * Evidence: movea.l a2,a0 / adda.w (a2)+,a0 / move.l a0,$32(a6); 42 runs
 */
void Op2_SetVector0(const u16 **script, Vm2Obj *self)
{
    self->vector[0] = rel_target(*script);
    (*script)++;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op2_SetVector1 - a2 op: script vector 1 = a relative script address (Op2_GotoVector1 /
 * Op2_GosubVector1, Op2_TickTimer0GotoV1).
 * In:  script = word offset (from the word itself)
 * Out: self->vector[1]; NEXT after the operand
 * Evidence: movea.l a2,a0 / adda.w (a2)+,a0 / move.l a0,$36(a6); 53 runs
 */
void Op2_SetVector1(const u16 **script, Vm2Obj *self)
{
    self->vector[1] = rel_target(*script);
    (*script)++;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
