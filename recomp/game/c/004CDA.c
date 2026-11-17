#include "game.h"

/* Op2_MoveTerrain / Op2_SubLiftVelY / Op2_TestAttackBoxes ($004CDA-$004DEB): this frame's move
 * through the scene terrain, the lift's correction to it, and the object's attack boxes. */

/* NEXT on a script held in a2 as a register value: the handler its word names, jumped to */
static void op_next_reg(uint32_t *a0, uint32_t *a2)
{
    PTR(const u16) script;
    script.v = *a2;
    *a0 = dk::code((int16_t)(uint16_t)*script.get());
    *a2 += 2;
    dk::jump(*a0);
}

/* the low word of a data register, the upper word kept (a .w operation) */
static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }
static uint32_t swap(uint32_t r) { return (r << 16) | (r >> 16); }
/* movem.w into a data register: the word sign-extended */
static uint32_t sext(uint16_t w) { return (uint32_t)(int32_t)(int16_t)w; }

/* jsr (Coll_MoveScene).l: it jumps to the scene's terrain routine, so its prototype hands every
 * register both ways (the routine's results d3-d7 come back) */
static void collide(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                    uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5,
                    uint32_t *a6)
{
    PTR(const Vm2Motion) mot;
    mot.v = *a3;
    Coll_MoveScene(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, mot, a4, a5, a6);
}

/* Op2_MoveTerrain - a2 op: moves the object by this frame's move through the scene terrain.
 * Without terrain boxes it just adds the move; box B (MOT_BOX_B) is collided first (its wall /
 * floor results kept); when B finds no floor and box A (MOT_BOX_A) is on, box A is collided, and a
 * wall / floor contact there stops the x / y velocity.
 * In:  d1 / d2 = the move (16.16, Op2_StepVelocity); mot (flags, the boxes' offsets and masks)
 * Out: self x / y (the fractions kept from the move); mot->terrain_code / wall_hit / contact;
 *      d1 / d2 = the integer move, then the terrain routine's; d4 / d5 = the box position, d6 /
 *      d7 its masks then the results; every register as the terrain routine leaves it; NEXT
 *      (VM2_Next when box A finds no floor)
 * Evidence: movem.l $3E(a6),d4-d5 / add.l / move.w d1,$40(a6) / swap x4 / sub.w; btst #2,(a3) /
 *           btst #1,(a3); jsr (Coll_MoveScene).l with d4 / d5 = position + box offset, movem.w
 *           $4C(a3) / $44(a3),d6-d7; clr.l $18(a3) / $1C(a3) on contact; 102 corpus runs;
 *           $133A8 `4B74 4CDA`. Prototype: every register in / out (the measured one saw
 *           Coll_MoveScene's jmp as its end, so the routine's d0 / d3 looked unchanged: verify
 *           failed 142 of 3,717 TAS calls on a later op reading them)
 */
void Op2_MoveTerrain(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                     uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4,
                     uint32_t *a5, uint32_t *a6)
{
    PTR(Vm2Motion) mot;
    mot.v = *a3;
    PTR(Vm2Obj) self;
    self.v = *a6;
    *d4 = (uint32_t)(uint16_t)self->x << 16 | self->x_frac;
    *d5 = (uint32_t)(uint16_t)self->y << 16 | self->y_frac;
    *d1 += *d4;                            /* the new position */
    *d2 += *d5;
    self->x_frac = (uint16_t)*d1;
    self->y_frac = (uint16_t)*d2;
    *d1 = swap(*d1);
    *d2 = swap(*d2);
    *d4 = swap(*d4);
    *d5 = swap(*d5);
    set_low_word(d1, (uint16_t)(*d1 - *d4)); /* the move in whole pixels */
    set_low_word(d2, (uint16_t)(*d2 - *d5));
    if (mot->flags & MOT_BOX_B) {
        set_low_word(d4, (uint16_t)(*d4 + mot->box_b_dx));
        set_low_word(d5, (uint16_t)(*d5 + mot->box_b_dy));
        *d6 = sext(mot->box_b_wall_mask);
        *d7 = sext(mot->box_b_floor_mask);
        collide(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
        mot->wall_hit = (uint16_t)*d6;
        mot->contact = (uint16_t)*d7;
        if ((uint16_t)*d7 != 0 || !(mot->flags & MOT_BOX_A)) {
            set_low_word(d4, (uint16_t)(*d4 - mot->box_b_dx));
            set_low_word(d5, (uint16_t)(*d5 - mot->box_b_dy));
            self->x = (int16_t)(uint16_t)*d4;
            self->y = (int16_t)(uint16_t)*d5;
            mot->terrain_code = (uint16_t)*d3;
            op_next_reg(a0, a2);
            return;
        }
        set_low_word(d4, (uint16_t)self->x);
        set_low_word(d5, (uint16_t)self->y);
    } else if (!(mot->flags & MOT_BOX_A)) {
        self->x = (int16_t)(self->x + (uint16_t)*d1);
        self->y = (int16_t)(self->y + (uint16_t)*d2);
        mot->terrain_code = 0;
        op_next_reg(a0, a2);
        return;
    }
    set_low_word(d4, (uint16_t)(*d4 + mot->box_a_dx));
    set_low_word(d5, (uint16_t)(*d5 + mot->box_a_dy));
    *d6 = sext(mot->box_a_wall_mask);
    *d7 = sext(mot->box_a_floor_mask);
    collide(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
    set_low_word(d4, (uint16_t)(*d4 - mot->box_a_dx));
    set_low_word(d5, (uint16_t)(*d5 - mot->box_a_dy));
    self->x = (int16_t)(uint16_t)*d4;
    self->y = (int16_t)(uint16_t)*d5;
    mot->terrain_code = (uint16_t)*d3;
    mot->wall_hit = (uint16_t)*d6;
    if ((uint16_t)*d6 != 0)
        mot->vel_x = 0;
    mot->contact = (uint16_t)*d7;
    if ((uint16_t)*d7 == 0) {
        dk::jump(at_VM2_Next);
        return;
    }
    mot->vel_y = 0;
    op_next_reg(a0, a2);
}

/* Op2_SubLiftVelY - a2 op: subtracts the lift section's vertical speed from this frame's y move
 * (an object moves relative to the lift).
 * In:  d2 = the y move; g_LiftVelY
 * Out: d2 -= g_LiftVelY; NEXT
 * Evidence: sub.l (g_LiftVelY).w,d2 / NEXT; g_LiftVelY ramped by $2410A; the zeppelin-dock scripts
 *           $33C10 `4D9C 4C52 3E58`; 20 corpus runs
 */
void Op2_SubLiftVelY(uint32_t *d2, const u16 **script)
{
    *d2 -= g_LiftVelY;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op2_TestAttackBoxes - a2 op: tests every attack box of the object against its targets
 * (Hitbox_TestAttackBox), unless the attack pause count is running, which it counts down instead.
 * The script's a2 is saved on the stack around the tests (and d7 / a1 around each of several).
 * In:  a4 = the VM block (delay, attack_count, attack[]); the registers the tests read
 * Out: the hits handled by the box callbacks (their registers); NEXT (VM2_Next with no boxes)
 * Evidence: tst.b $1(a4) / subq.b; move.w $A(a4),d7 / subq.w #1 / bmi.w VM2_Next / bne; one box:
 *           move.l a2,-(a7) / movea.w $C(a4),a0 / bsr.w Hitbox_TestAttackBox; more: pea (a2) /
 *           lea $C(a4),a1 / movea.w (a1)+,a0 / movem.l d7/a1,-(a7) / bsr / movem.l (a7)+,d7/a1 /
 *           dbra; PlayerAtk_HitPause sets $1(a4); 114 corpus runs; $1812E `4DA4 448A`
 */
void Op2_TestAttackBoxes(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                         uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a3,
                         uint32_t *a4, uint32_t a5, uint32_t *a6)
{
    PTR(Vm2Block) vm;
    vm.v = *a4;
    if (vm->delay != 0) {
        vm->delay = (uint8_t)(vm->delay - 1);
        op_next_reg(a0, a2);
        return;
    }
    set_low_word(d7, (uint16_t)(vm->attack_count - 1));
    if ((int16_t)*d7 < 0) {
        dk::jump(at_VM2_Next);
        return;
    }
    dk::push_long(*a2);
    if ((uint16_t)*d7 == 0) {
        *a0 = sext((uint16_t)vm->attack[0]);
        Hitbox_TestAttackBox(d0, d1, d2, d3, d4, d5, d6, d7, *a0, a1, a3, a4, a5, a6);
    } else {
        *a1 = ADDR68(&vm->attack[0]);
        do {
            PTR(const s16) handle;
            handle.v = *a1;
            *a0 = sext((uint16_t)*handle.get());
            *a1 += 2;
            dk::push_long(*a1);            /* movem.l d7/a1,-(a7): d7 at the lower address */
            dk::push_long(*d7);
            Hitbox_TestAttackBox(d0, d1, d2, d3, d4, d5, d6, d7, *a0, a1, a3, a4, a5, a6);
            *d7 = dk::pop_long();
            *a1 = dk::pop_long();
            set_low_word(d7, (uint16_t)(*d7 - 1));
        } while ((uint16_t)*d7 != 0xFFFF);
    }
    *a2 = dk::pop_long();
    op_next_reg(a0, a2);
}
