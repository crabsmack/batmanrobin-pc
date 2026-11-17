#include "game.h"

/* $004EA8-$004F41: Op2_TestAttackBoxAtDrawn, Op2_PlaceBodyBoxes (each frame the object's body
 * boxes, the ones it is struck through, get their world rectangle and join their collision list)
 * and Hitbox_LinkByKind, the kind jump table that links a box. The VM block's byte +$00 is the
 * body-box off count (Op2_PlaceBodyBoxes_Off unlinks them while it runs). */

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
/* movem.w into a data register: the word sign-extended */
static uint32_t sext(int16_t w) { return (uint32_t)(int32_t)w; }

/* the handle word at a register address, sign-extended (movea.w (a1)+,a0 / movea.w $C(a4,d0.w),a0) */
static uint32_t handle_at(uint32_t address)
{
    PTR(const s16) h;
    h.v = address;
    return sext(*h.get());
}

/* Op2_TestAttackBoxAtDrawn - a2 op: tests one attack box, chosen by index, at the object's drawn
 * position (Hitbox_TestAttackBoxAt), unless the attack pause count is running (counted down).
 * In:  script (a2) = word index; a4 = the VM block (delay, attack_count, attack[]); a6 = the object
 * Out: d0 low word = the index (doubled when tested); a0 = the box; d1 / d2 = the drawn position;
 *      the test's registers; NEXT (VM2_Next for an index out of range)
 * Evidence: move.w (a2)+,d0 / tst.b $1(a4) / subq.b; cmp.w $A(a4),d0 / bge.w VM2_Next / add.w d0,d0 /
 *           movea.w $C(a4,d0.w),a0 / pea (a2) / move.w $4E(a6),d1 / move.w $50(a6),d2 /
 *           bsr.w Hitbox_TestAttackBoxAt / movea.l (a7)+,a2; 5 corpus runs; $1E7BE `4EA8 0000`
 */
void Op2_TestAttackBoxAtDrawn(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                              uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a3,
                              uint32_t *a4, uint32_t a5, uint32_t *a6)
{
    PTR(Vm2Block) vm;
    vm.v = *a4;
    PTR(const u16) script;
    script.v = *a2;
    set_low_word(d0, *script.get());
    *a2 += 2;
    if (vm->delay != 0) {
        vm->delay = (uint8_t)(vm->delay - 1);
        op_next_reg(a0, a2);
        return;
    }
    if ((int16_t)*d0 >= (int16_t)vm->attack_count) {
        dk::jump(at_VM2_Next);
        return;
    }
    set_low_word(d0, (uint16_t)(*d0 * 2));
    *a0 = handle_at(ADDR68(&vm->attack[0]) + (uint32_t)(int32_t)(int16_t)*d0);
    dk::push_long(*a2);
    PTR(const Vm2Obj) self;
    self.v = *a6;
    set_low_word(d1, (uint16_t)self->draw_x);
    set_low_word(d2, (uint16_t)self->draw_y);
    Hitbox_TestAttackBoxAt(d0, d1, d2, d3, d4, d5, d6, d7, *a0, a1, a3, a4, a5, a6);
    *a2 = dk::pop_long();
    op_next_reg(a0, a2);
}

/* Op2_PlaceBodyBoxes - a2 op: places every body box for this frame (Op2_PlaceBodyBoxes_Next);
 * while the off count runs it unlinks them instead (Op2_PlaceBodyBoxes_Off).
 * In:  a4 = the VM block (+$00 the off count, body_count, body[]); a6 = the object
 * Out: d0 = body_count - 1, a1 = &body[0], d1 / d2 = the object's x / y; then the loop's
 *      (VM2_Next with no boxes)
 * Evidence: move.w $2(a4),d0 / subq.w #1 / bmi.w VM2_Next / lea $4(a4),a1 / tst.b (a4) /
 *           bne.s Op2_PlaceBodyBoxes_Off / move.w $3E(a6),d1 / move.w $42(a6),d2; 151 corpus runs;
 *           $1812C `4EDA 4DA4 448A`
 */
void Op2_PlaceBodyBoxes(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                        uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                        uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    PTR(const Vm2Block) vm;
    vm.v = *a4;
    set_low_word(d0, (uint16_t)(vm->body_count - 1));
    if ((int16_t)*d0 < 0) {
        dk::jump(at_VM2_Next);
        return;
    }
    *a1 = ADDR68(&vm->body[0]);
    if (vm->body_off != 0) {                 /* the off count */
        dk::jump(at_Op2_PlaceBodyBoxes_Off);
        return;
    }
    PTR(const Vm2Obj) self;
    self.v = *a6;
    set_low_word(d1, (uint16_t)self->x);
    set_low_word(d2, (uint16_t)self->y);
    Op2_PlaceBodyBoxes_Next(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
}

/* Op2_PlaceBodyBoxes_Next - the loop head of Op2_PlaceBodyBoxes: the next body-box handle from a1,
 * then Op2_PlaceBodyBoxes_Place.
 * In:  a1 = the next handle; d0 = boxes left - 1; d1 / d2 = the position
 * Out: a0 = the box, a1 + 2; then Op2_PlaceBodyBoxes_Place's
 * Evidence: movea.w (a1)+,a0, falls into $4EF6; the dbra target of the loop. Prototype: no
 *           measured contract (a branch target only); Op2_PlaceBodyBoxes_Place's registers
 */
void Op2_PlaceBodyBoxes_Next(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                             uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                             uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *a0 = handle_at(*a1);
    *a1 += 2;
    Op2_PlaceBodyBoxes_Place(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
}

/* Op2_PlaceBodyBoxes_Place - places body box a0 at d1 / d2: the owner's energy copied in, the spec
 * rectangle mirrored by the sprite's flip bits and placed, the box linked by its kind
 * (Hitbox_LinkByKind's table) when it is on no list; then the next handles, NEXT after the last.
 * In:  a0 = the box; d1 / d2 = the position; d0 = boxes left - 1; a1 = the next handle;
 *      a5 = the sprite block; a6 = the object
 * Out: box energy / left / top / right / bottom; d3 = the kind, d4-d7 = the rectangle (the last
 *      box's); d0 = $FFFF; NEXT
 * Evidence: move.w $52(a6),$1E(a0) / movem.w $6(a0),d3-d7 / the flip code of
 *           Hitbox_TestAttackBoxAt / movem.w d4-d7,$12(a0) / tst.w $2(a0) / bne /
 *           jsr Hitbox_LinkByKind(pc,d3.w) / dbra d0,$4EF4; beq.w here from Op2_PlaceBodyBox(AtDrawn)
 */
void Op2_PlaceBodyBoxes_Place(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                              uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                              uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    (void)a3, (void)a4;
    PTR(const Vm2Obj) self;
    self.v = *a6;
    PTR(const Vm2Sprite) spr;
    spr.v = *a5;
    for (;;) {
        PTR(HitBox) box;
        box.v = *a0;
        box->energy = self->energy;
        *d3 = sext(box->kind);
        *d4 = sext(box->spec_left);
        *d5 = sext(box->spec_top);
        *d6 = sext(box->spec_right);
        *d7 = sext(box->spec_bottom);
        if (spr->attr & 0x0800) {         /* H flip: left / right mirrored */
            set_low_word(d4, (uint16_t)-*d4);
            set_low_word(d6, (uint16_t)-*d6);
            uint32_t t = *d4;
            *d4 = *d6;
            *d6 = t;
        }
        if (spr->attr & 0x1000) {         /* V flip: top / bottom mirrored */
            set_low_word(d5, (uint16_t)-*d5);
            set_low_word(d7, (uint16_t)-*d7);
            uint32_t t = *d5;
            *d5 = *d7;
            *d7 = t;
        }
        set_low_word(d4, (uint16_t)(*d4 + *d1));
        set_low_word(d6, (uint16_t)(*d6 + *d1));
        set_low_word(d5, (uint16_t)(*d5 + *d2));
        set_low_word(d7, (uint16_t)(*d7 + *d2));
        box->left = (int16_t)(uint16_t)*d4;
        box->top = (int16_t)(uint16_t)*d5;
        box->right = (int16_t)(uint16_t)*d6;
        box->bottom = (int16_t)(uint16_t)*d7;
        if (box->prev == 0)               /* on no list yet */
            HitboxLinkFn(at_Hitbox_LinkByKind + (uint32_t)(int32_t)(int16_t)*d3, *a0, *a1);
        set_low_word(d0, (uint16_t)(*d0 - 1));
        if ((uint16_t)*d0 == 0xFFFF)
            break;
        *a0 = handle_at(*a1);               /* Op2_PlaceBodyBoxes_Next */
        *a1 += 2;
    }
    op_next_reg(a0, a2);
}

/* Hitbox_LinkByKind - the jump table by a box's kind word (jsr $4F3E(pc,d3.w)): kind 0 appends the
 * box to g_PlayerBoxList (Hitbox_LinkPlayerList); entry 8 is Hitbox_LinkByKind_Enemy.
 * In:  a0 = the box; a1 passed on
 * Out: the box linked (the append routine's rts)
 * Evidence: bra.w Hitbox_LinkPlayerList; jsr $4F3E(pc,d3.w) in Op2_PlaceBodyBoxes
 */
void Hitbox_LinkByKind(uint32_t a0, uint32_t a1)
{
    (void)a0, (void)a1;
    dk::jump(at_Hitbox_LinkPlayerList);
}
