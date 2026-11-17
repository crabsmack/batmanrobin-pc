#include "game.h"

/* The target-point offset ops and Op2_TargetSelf ($001708-$001735). */

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_AddTargetY - a2 op: moves the target point's y by a signed word (the y the Y seek ops
 * compare against).
 * In:  script = word dy
 * Out: mot->point_y; delta = dy; NEXT
 * Evidence: move.w (a2)+,d0 / add.w d0,$16(a3) / NEXT; prototype: a2 an output (its NEXT hands the
 *           advanced script to the next op; it was `clobbers(a2)`); entered in 3 corpus runs incl.
 *           the TAS (frames 116492-118792)
 */
void Op2_AddTargetY(uint16_t *delta, const u16 **script, Vm2Motion *mot)
{
    uint32_t handler;
    *delta = *(*script)++;
    mot->point_y = (int16_t)(mot->point_y + *delta);
    next(&handler, script);
}

/* Op2_AddTargetX - a2 op: moves the target point's x by a signed word.
 * In:  script = word dx
 * Out: mot->point_x; NEXT
 * Evidence: move.w (a2)+,d0 / add.w d0,$14(a3) / NEXT; entered in 2 corpus runs incl. the TAS
 */
void Op2_AddTargetX(const u16 **script, Vm2Motion *mot)
{
    uint32_t handler;
    mot->point_x = (int16_t)(mot->point_x + *(*script)++);
    next(&handler, script);
}

/* Op2_TargetSelf - a2 op: the object's own position becomes the target point (without
 * Mot_SetTarget's direction bits).
 * In:  self->x / y
 * Out: mot->point_x / point_y; NEXT
 * Evidence: move.w $3E(a6),$14(a3) / move.w $42(a6),$16(a3) / NEXT; entered in 64 corpus runs
 */
void Op2_TargetSelf(uint32_t *handler, const u16 **script, Vm2Motion *mot, const Vm2Obj *self)
{
    mot->point_x = self->x;
    mot->point_y = self->y;
    next(handler, script);
}
