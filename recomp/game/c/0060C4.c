#include "game.h"

/* VM4_Fork and Op4_ChildAddOwnVel ($0060C4-$006139): the cheap spawn of the Op4_Fork* ops (one
 * script word, the child starts exactly at d2-d5), and carrying the spawner's motion over to
 * the child. */

/* VM4_Fork - spawns a child a4 object running script a3: a pool block on the object list
 * (Obj_New, handler VM4_RunObject), the owner id and sprite attributes inherited, animation,
 * frames, box and depth cleared, radius / heading / x / y = d2-d5, velocities and accelerations 0.
 * In:  a3 = the child's script; a6 = the parent (an object or g_SpawnCtx); d2-d5 = radius /
 *      heading / x / y (16.16)
 * Out: a5 = the child (the parent when the pool is full); d0 = 0 (-1 full); a0 / a1 as Obj_New
 *      leaves them, a0 past the motion longs on success; Z set on success, clear when full
 *      (both the tst of d0: moveq / move.l d0 last)
 * Evidence: movea.l a6,a5 / lea ($53B4).w,a0 / bsr Obj_New / exg a5,a6 / bne rts / move.l a3,$8(a5)
 *           ... 12 longs from $26(a5); callers Op4_Fork / ForkAbs / ForkOrBranch / ForkAbsAndEnd /
 *           ForkRestAndEnd test Z
 */
void VM4_Fork(uint32_t *d0, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t *a0, uint32_t *a1,
              uint32_t a3, uint32_t *a5, const Vm4Obj *parent, dk::Flags *ccr)
{
    uint32_t obj = dk::addr(parent);
    *a0 = at_VM4_RunObject;
    Obj_New(d0, a0, a1, &obj);
    *a5 = obj;   /* exg a5,a6: the new object, or still the parent */
    *ccr = dk::Flags::test(*d0, 4);
    if (*d0 != 0)
        return;
    PTR(Vm4Obj) child;
    child.v = obj;
    child->resume.v = a3;
    child->attr = parent->attr;
    child->owner_id = parent->owner_id;
    child->anim_tick = 0;   /* move.w d0,$1C(a5): the ticker and its rate */
    child->anim_rate = 0;
    child->frame = 0;
    child->frame_shown = 0;
    child->box = 0;
    child->depth = 0;
    vm4_set_fix(child->radius, child->radius_frac, d2);
    vm4_set_fix(child->heading, child->heading_frac, d3);
    vm4_set_fix(child->x, child->x_frac, d4);
    vm4_set_fix(child->y, child->y_frac, d5);
    child->vel_radius = 0;
    child->vel_heading = 0;
    child->vel_x = 0;
    child->vel_y = 0;
    child->acc_radius = 0;
    child->acc_heading = 0;
    child->acc_x = 0;
    child->acc_y = 0;
    *a0 = ADDR68(&child->draw_x);   /* lea $26(a5),a0 past the 12 longs */
}

/* Op4_ChildAddOwnVel - a4 op: adds this object's velocities (radius, heading, x, y) to the last
 * spawned child's, so the child carries the spawner's motion.
 * In:  script (no operands); child = a5; self = a6
 * Out: d0 = self's y velocity; the child's velocities; NEXT
 * Evidence: move.l $36(a6),d0 / add.l d0,$36(a5) ... $42; script uses $2056C, $28720 (after
 *           Op4_ForkAbs: `60AC ... 6116`), $28F50; 20 corpus runs
 */
void Op4_ChildAddOwnVel(uint32_t *d0, const u16 **script, Vm4Obj *child, const Vm4Obj *self)
{
    *d0 = (uint32_t)(int32_t)self->vel_radius;
    vm4_add_long(child->vel_radius, *d0);
    *d0 = (uint32_t)(int32_t)self->vel_heading;
    vm4_add_long(child->vel_heading, *d0);
    *d0 = (uint32_t)(int32_t)self->vel_x;
    vm4_add_long(child->vel_x, *d0);
    *d0 = (uint32_t)(int32_t)self->vel_y;
    vm4_add_long(child->vel_y, *d0);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
