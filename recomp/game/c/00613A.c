#include "game.h"

/* VM4_SpawnChild and VM4_ReinitSelf ($00613A-$006207): the spawn core of Op4_Spawn* and
 * Op4_ReplaceSelf. The operands after the op word: word failure offset, word child-script offset
 * (from its own word), word parameter-table offset (from its own word, 0 = none). The table is
 * four (value, velocity, acceleration) triples of longs for x, y, radius and heading; the values
 * add to d4 / d5 / d2 / d3. */

/* the parameter table's triples, in table order (x +$00, y +$0C, radius +$18, heading +$24) */
typedef struct Vm4SpawnTriple {
    s32 value;
    s32 vel;
    s32 acc;
} Vm4SpawnTriple;
typedef struct Vm4SpawnTable {
    Vm4SpawnTriple x, y, radius, heading;
} Vm4SpawnTable;
static_assert(offsetof(Vm4SpawnTable, radius) == 0x18, "Vm4SpawnTable.radius");
static_assert(offsetof(Vm4SpawnTable, heading) == 0x24, "Vm4SpawnTable.heading");

/* a word operand at a script address held in a register */
static uint16_t word_at(uint32_t a)
{
    PTR(const u16) p;
    p.v = a;
    return *p;
}

/* VM4_SpawnChild - the spawn core of Op4_Spawn*: allocates a child a4 object (Obj_New, handler
 * VM4_RunObject) and sets it up (VM4_SpawnChild_Init). A full pool drops the op's return: the
 * failure offset 0 ends the run (rts to the VM's caller), else the script branches there (NEXT).
 * In:  a4 = the 3 operand words; d2-d5 = radius / heading / x / y (16.16); a6 = the parent
 * Out: a5 = the child; rts to the op with VM4_SpawnChild_Init's registers. Full: d0 = the failure
 *      offset (and its tst), a5 = the parent, a0 / a1 as Obj_New left them, the op's return
 *      dropped
 * Evidence: movea.l a6,a5 / lea ($53B4).w,a0 / bsr Obj_New / exg a5,a6 / beq / addq.l #4,a7 /
 *           move.w (a4),d0 / beq rts / adda.w d0,a4 / NEXT; callers Op4_Spawn, Op4_SpawnAddVel,
 *           Op4_SpawnRunChild
 */
void VM4_SpawnChild(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t *a0,
                    uint32_t *a1, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t a6, dk::Flags *ccr)
{
    uint32_t obj = a6;
    *a0 = at_VM4_RunObject;
    Obj_New(d0, a0, a1, &obj);
    *a5 = obj;   /* exg a5,a6: the new object, or still the parent */
    if (*d0 == 0) {
        PTR(Vm4Obj) child;
        child.v = *a5;
        PTR(const Vm4Obj) parent;
        parent.v = a6;
        VM4_SpawnChild_Init(d0, d1, d2, d3, d4, d5, a0, a3, a4, child.get(), parent.get(), ccr);
        return;
    }
    dk::drop_return();   /* addq.l #4,a7: no return to the spawn op */
    *d0 = (*d0 & 0xFFFF0000u) | word_at(*a4);
    *ccr = dk::Flags::test(*d0, 2);
    if ((uint16_t)*d0 == 0)
        return;          /* rts: ends the run */
    *a4 += (uint32_t)(int32_t)(int16_t)(uint16_t)*d0;
    *a0 = dk::code((int16_t)word_at(*a4));
    *a4 += 2;
    dk::jump(*a0);
}

/* VM4_ReinitSelf - Op4_ReplaceSelf's entry into VM4_SpawnChild: frees the object's collision box
 * (if any) and re-initialises the object itself (a5 = a6) from the spawn operands; animation,
 * frames, attributes and owner stay.
 * In:  a4 = Op4_ReplaceSelf's spawn operands; a6 = the object; a5 = a6 (Op4_ReplaceSelf sets it);
 *      a1 passed to Hitbox_Unlink; d2-d5
 * Out: the object re-initialised (VM4_SpawnChild_InitMotion's registers); rts (to VM4_ResumeA5,
 *      which Op4_ReplaceSelf pushed)
 * Evidence: move.w $14(a6),d0 / beq / movea.w d0,a0 / bsr Hitbox_Unlink / bsr ObjPool_Free /
 *           movea.l a6,a5 / bra.s VM4_SpawnChild_InitMotion; bra.w from Op4_ReplaceSelf ($557E)
 */
void VM4_ReinitSelf(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t *a0,
                    uint32_t a1, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t a6, dk::Flags *ccr)
{
    PTR(const Vm4Obj) self;
    self.v = a6;
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)self->box;
    if (self->box != 0) {
        *a0 = (uint32_t)(int32_t)self->box;
        Hitbox_Unlink(*a0, a1);
        ObjPool_Free(*a0);
        *a5 = a6;
    }
    PTR(Vm4Obj) obj;
    obj.v = *a5;
    VM4_SpawnChild_InitMotion(d0, d1, d2, d3, d4, d5, a0, a3, a4, obj.get(), ccr);
}

/* VM4_SpawnChild_Init - VM4_SpawnChild after a successful allocation: clears the child's
 * animation ticker / rate and frame words, inherits the parent's sprite attributes and owner
 * id, then VM4_SpawnChild_InitMotion.
 * In:  a5 = the child; a6 = the parent; a4 = the spawn operands; d2-d5
 * Out: as VM4_SpawnChild_InitMotion
 * Evidence: clr.w $1C(a5) / clr.w $1E(a5) / clr.w $20(a5) / move.w $22(a6),$22(a5) /
 *           move.w $10(a6),$10(a5), falling into $6182; beq.s here from VM4_SpawnChild
 */
void VM4_SpawnChild_Init(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                         uint32_t *a0, uint32_t *a3, uint32_t *a4, Vm4Obj *child, const Vm4Obj *parent,
                         dk::Flags *ccr)
{
    child->anim_tick = 0;   /* clr.w $1C(a5): the ticker and its rate */
    child->anim_rate = 0;
    child->frame = 0;
    child->frame_shown = 0;
    child->attr = parent->attr;
    child->owner_id = parent->owner_id;
    VM4_SpawnChild_InitMotion(d0, d1, d2, d3, d4, d5, a0, a3, a4, child, ccr);
}

/* VM4_SpawnChild_InitMotion - the shared tail of VM4_SpawnChild and VM4_ReinitSelf: clears the
 * box and depth, sets the script from the child-script offset and the motion from d2-d5, plus
 * the parameter table's values, velocities and accelerations when there is one.
 * In:  a5 = the object; a4 = the spawn operands; d2-d5
 * Out: a4 past the operands; a3 = the script, then the table; d1 = the failure offset (high
 *      word) and the table offset; d0 = 0, or the last sum (y); a0 past the 12 motion longs; the
 *      condition codes of the last move.l; rts
 * Evidence: moveq #0,d0 / move.w d0,$14(a5) / move.w d0,$24(a5) / lea $26(a5),a0 /
 *           move.l (a4)+,d1 / lea -$2(a4,d1.w),a3 / move.l a3,$8(a5) / move.w (a4)+,d1 / bne;
 *           table: move.l $18(a3),d0 / add.l d2,d0 ... move.l $14(a3),(a0)+
 */
void VM4_SpawnChild_InitMotion(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                               uint32_t *a0, uint32_t *a3, uint32_t *a4, Vm4Obj *obj, dk::Flags *ccr)
{
    *d0 = 0;
    obj->box = 0;
    obj->depth = 0;
    *a0 = ADDR68(&obj->draw_x);   /* past the 12 longs from $26 */
    /* move.l (a4)+,d1: the failure offset (high word) and the child-script offset (low word) */
    *d1 = (uint32_t)word_at(*a4) << 16 | word_at(*a4 + 2);
    *a4 += 4;
    *a3 = *a4 - 2 + (uint32_t)(int32_t)(int16_t)(uint16_t)*d1;
    obj->resume.v = *a3;
    *d1 = (*d1 & 0xFFFF0000u) | word_at(*a4);   /* the table offset */
    *a4 += 2;
    if ((uint16_t)*d1 == 0) {
        vm4_set_fix(obj->radius, obj->radius_frac, d2);
        vm4_set_fix(obj->heading, obj->heading_frac, d3);
        vm4_set_fix(obj->x, obj->x_frac, d4);
        vm4_set_fix(obj->y, obj->y_frac, d5);
        obj->vel_radius = 0;
        obj->vel_heading = 0;
        obj->vel_x = 0;
        obj->vel_y = 0;
        obj->acc_radius = 0;
        obj->acc_heading = 0;
        obj->acc_x = 0;
        obj->acc_y = 0;
        *ccr = dk::Flags::test(0, 4);
        return;
    }
    *a3 = *a4 - 2 + (uint32_t)(int32_t)(int16_t)(uint16_t)*d1;
    PTR(const Vm4SpawnTable) table;
    table.v = *a3;
    *d0 = (uint32_t)(int32_t)table->radius.value + d2;
    vm4_set_fix(obj->radius, obj->radius_frac, *d0);
    *d0 = (uint32_t)(int32_t)table->heading.value + d3;
    vm4_set_fix(obj->heading, obj->heading_frac, *d0);
    *d0 = (uint32_t)(int32_t)table->x.value + d4;
    vm4_set_fix(obj->x, obj->x_frac, *d0);
    *d0 = (uint32_t)(int32_t)table->y.value + d5;
    vm4_set_fix(obj->y, obj->y_frac, *d0);
    obj->vel_radius = table->radius.vel;
    obj->vel_heading = table->heading.vel;
    obj->vel_x = table->x.vel;
    obj->vel_y = table->y.vel;
    obj->acc_radius = table->radius.acc;
    obj->acc_heading = table->heading.acc;
    obj->acc_x = table->x.acc;
    obj->acc_y = table->y.acc;
    *ccr = dk::Flags::test((uint32_t)(int32_t)table->y.acc, 4);
}

/* Op4_SetXYFromDraw - a4 op: sets the integer words of the object's x / y to the drawn x / y words
 * in d4 / d5 (keeps a terrain-corrected position); the fractions stay.
 * In:  script (no operands); d4 / d5 low words = the position; self = a6
 * Out: self->x / y; NEXT
 * Evidence: move.w d4,$2E(a6) / move.w d5,$32(a6) / NEXT; script use $26D68 (after
 *           Op4_AltTerrainMove `6576 0002 61FC`); 3 corpus runs
 */
void Op4_SetXYFromDraw(uint16_t d4, uint16_t d5, const u16 **script, Vm4Obj *self)
{
    self->x = (int16_t)d4;
    self->y = (int16_t)d5;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
