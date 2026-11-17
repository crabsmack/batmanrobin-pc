#include "game.h"

/* Op4_SetDamage / Op4_ChildSetDamage and the spawn-position jitters ($0056E6-$005715). */

/* NEXT: the handler the script's next word names, jumped to */
static void op_next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* a random offset in [-w/2, w/2) units of the integer word (16.16): Rand_Next's signed low word
   times the operand w (muls.w (a4)+,d0) */
static uint32_t jitter(const u16 **script)
{
    int16_t r = (int16_t)(uint16_t)(uint32_t)Rand_Next();
    return (uint32_t)((int32_t)r * (int16_t)(uint16_t)*(*script)++);
}

/* Op4_SetDamage - a4 op: sets the damage the object's hit natives deal (Shot_DamageTarget adds it
 * to the struck box and takes it from the box's energy).
 * In:  script = word damage
 * Out: self->damage; NEXT
 * Evidence: move.w (a4)+,$12(a6); script uses $1AB6C ($10), $26B86, $26CF4; 75 corpus runs
 */
void Op4_SetDamage(uint32_t *handler, const u16 **script, Vm4Obj *self)
{
    self->damage = *(*script)++;
    op_next(handler, script);
}

/* Op4_ChildSetDamage - a4 op: sets the damage of the last spawned child.
 * In:  script = word damage; child = a5
 * Out: child->damage; the move.w's condition codes; NEXT
 * Evidence: move.w (a4)+,$12(a5); 159 a4-script uses, e.g. $25B7E (`56EE 0010` then a yield),
 *           $1F0E4, the shot scripts; 151 corpus runs
 */
void Op4_ChildSetDamage(uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr)
{
    uint16_t damage = *(*script)++;
    child->damage = damage;
    *ccr = dk::Flags::test(damage, 2, ccr->x);
    op_next(handler, script);
}

/* Op4_JitterSpawnX - a4 op: adds a random [-w/2, w/2) px to the spawn x register d4 (16.16) for
 * the next spawn op.
 * In:  script = word w; d4 = spawn x
 * Out: d4 += the offset; NEXT
 * Evidence: bsr Rand_Next / muls.w (a4)+,d0 / add.l d0,d4; script uses $1A4A4 (`56F6 0037 5606`),
 *           $1AC96; 8 corpus runs
 */
void Op4_JitterSpawnX(uint32_t *d4, const u16 **script)
{
    *d4 += jitter(script);
    uint32_t handler;
    op_next(&handler, script);
}

/* Op4_JitterSpawnXY - a4 op: adds random [-wx/2, wx/2) / [-wy/2, wy/2) px to the spawn position
 * registers d4/d5.
 * In:  script = word wx, word wy; d4/d5 = spawn x/y (16.16)
 * Out: d4/d5 += the offsets; NEXT
 * Evidence: two bsr Rand_Next / muls.w (a4)+,d0 / add.l to d4 and d5; script uses $14A18
 *           (`5702 0008 0008`), $1A404, $1A454 before spawn ops; 63 corpus runs
 */
void Op4_JitterSpawnXY(uint32_t *d4, uint32_t *d5, const u16 **script)
{
    *d4 += jitter(script);
    *d5 += jitter(script);
    uint32_t handler;
    op_next(&handler, script);
}
