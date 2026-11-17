#include "game.h"

/* Obj_SetBodyBoxes, Op2_BranchStruck and Op2_BranchNotStruck ($00385A-$00390B). */

/* the low word of a data register, the upper word kept (a .w operation) */
static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* dbra on a counter's low word: true while it has not gone past 0 to -1 */
static bool dbra(uint16_t *count) { return --*count != 0xFFFFu; }

/* a typed pointer from a 68000 address held in a register */
template <class T> static T *at(uint32_t a)
{
    PTR(T) p;
    p.v = a;
    return p;
}

/* Obj_SetBodyBoxes's body, on a typed script, VM block and object */
static void rebuild_body_boxes(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                               uint32_t *d6, uint16_t *count, uint32_t *a1, const u16 **script, Vm2Block *vm,
                               Vm2Obj *self)
{
    vm->body_off = 0;
    s16 *slot = &vm->body[0];
    *d6 = 0;
    uint32_t *spec_regs[5] = { d1, d2, d3, d4, d5 };
    bool failed = false;
    while (dbra(count)) {
        set_low_word(d0, (uint16_t)*slot++);
        uint32_t a0 = (uint32_t)(int32_t)(int16_t)(uint16_t)*d0;
        if ((uint16_t)*d0 == 0) {
            ObjPool_Alloc(d0, &a0);
            if (*d0 != 0) {   /* the pool is empty (d0 = -1, Z clear) */
                failed = true;
                break;
            }
            slot[-1] = (int16_t)(uint16_t)a0;
            dk::handle<HitBox>((int16_t)(uint16_t)a0)->prev = 0;   /* on no list yet */
        }
        HitBox *box = dk::handle<HitBox>((int16_t)(uint16_t)a0);
        box->on_hit = at_Native_NoOp;
        box->flags = 0;
        box->unk_3D = 0;
        box->damage = 0;
        box->owner = dk::handle_of(self);
        box->id = self->owner_id;
        set_low_word(d0, self->serial);
        for (int i = 0; i < 5; i++)
            *spec_regs[i] = (uint32_t)(int32_t)(int16_t)(uint16_t)*(*script)++;
        box->serial = (uint16_t)*d0;
        box->kind = (uint16_t)*d1;
        box->spec_left = (int16_t)(uint16_t)*d2;
        box->spec_top = (int16_t)(uint16_t)*d3;
        box->spec_right = (int16_t)(uint16_t)*d4;
        box->spec_bottom = (int16_t)(uint16_t)*d5;
        set_low_word(d0, *(*script)++);
        if ((uint16_t)*d0 != 0) {
            const u32 *callback = (const u32 *)*script;
            box->on_hit = *callback;
            *script = (const u16 *)(callback + 1);
            /* subq.w #3 / blt: the exact difference below 0 */
            int32_t args = (int32_t)(int16_t)(uint16_t)*d0 - 3;
            set_low_word(d0, (uint16_t)args);
            if (args >= 0) {
                u16 *arg = &box->hit_damage;
                uint16_t n = (uint16_t)*d0;
                do {
                    *arg++ = *(*script)++;
                } while (dbra(&n));
                set_low_word(d0, n);
            }
        }
        set_low_word(d6, (uint16_t)(*d6 + 1));
    }
    if (failed) {
        /* skip this and the remaining specs: each is $C bytes plus 2 per callback word */
        do {
            set_low_word(d0, (*script)[5]);
            set_low_word(d0, (uint16_t)(*d0 + *d0));
            *script = (const u16 *)((const u8 *)*script + 0xC + (int16_t)(uint16_t)*d0);
        } while (dbra(count));
    }
    uint16_t surplus = (uint16_t)(vm->body_count - (uint16_t)*d6);
    vm->body_count = (uint16_t)*d6;
    *count = (uint16_t)(surplus - 1);
    if (!(*count & 0x8000u)) {
        do {
            uint32_t a0 = (uint32_t)(int32_t)(int16_t)*slot;
            *slot++ = 0;
            if (dk::handle<HitBox>((int16_t)(uint16_t)a0)->prev != 0)
                Hitbox_Unlink(a0, dk::addr(slot));
            ObjPool_Free(a0);
        } while (dbra(count));
    }
    *a1 = dk::addr(slot);
}
/* Obj_SetBodyBoxes - rebuilds the object's body-box set (the boxes it can be struck through)
 * from `count` inline specs, as Obj_SetAttackBoxes does, also clearing each box's damage taken (a
 * new box starts on no list); the boxes beyond the new count are unlinked from their list (when
 * linked) and freed. A failed allocation skips the remaining specs (and frees from the slot after
 * the failed one: the original's off-by-one, kept).
 * In:  count (d7) = specs; script = specs (words kind, left, top, right, bottom, word n, callback
 *      + argument words); vm, self
 * Out: vm->body_off = 0; vm->body_count, vm->body[]; d0-d6, d7, a1 as the asm leaves them (d1-d5 =
 *      the last spec's words, sign-extended); script past the specs
 * Evidence: clr.b (a4) / lea $4(a4),a1 / ObjPool_Alloc / clr.w $2(a0) / clr.w $20(a0) / movem.w
 *           d0-d5,$4(a0) ... / tst.w $2(a0) / Hitbox_Unlink / ObjPool_Free; 153 runs
 */
void Obj_SetBodyBoxes(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                      uint32_t *d6, uint16_t *count, uint32_t *a1, uint32_t *a2, uint32_t a4, uint32_t a6)
{
    const u16 *script = at<const u16>(*a2);
    rebuild_body_boxes(d0, d1, d2, d3, d4, d5, d6, count, a1, &script, at<Vm2Block>(a4), at<Vm2Obj>(a6));
    *a2 = dk::addr(script);
}

/* Op2_BranchStruck - a2 op: branches when one of the object's body boxes was struck since the
 * last test (Obj_TestStruck: the box goes to g_StruckBox).
 * In:  script = word offset
 * Out: d0/d7/a1 as Obj_TestStruck leaves them; struck: Op2_Goto, else NEXT after the offset
 * Evidence: bsr.w Obj_TestStruck / bne.w Op2_Goto / addq.l #2,a2 / NEXT; 137 runs
 */
void Op2_BranchStruck(uint32_t *d0, uint16_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, Vm2Block *vm)
{
    Obj_TestStruck(d0, d7, a1, vm);
    if (*d0 != 0) {   /* moveq #1 / moveq #0: Z = (d0 == 0) */
        dk::jump(at_Op2_Goto);
        return;
    }
    (*script)++;
    *a0 = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*a0);
}

/* Op2_BranchNotStruck - a2 op: branches when none of the object's body boxes was struck since the
 * last test.
 * In:  script = word offset
 * Out: d0/d7/a1 as Obj_TestStruck leaves them (struck: g_StruckBox set); not struck: Op2_Goto,
 *      else NEXT after the offset
 * Evidence: bsr.w Obj_TestStruck / beq.w Op2_Goto / addq.l #2,a2 / NEXT; 145 runs
 */
void Op2_BranchNotStruck(uint32_t *d0, uint16_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, Vm2Block *vm)
{
    Obj_TestStruck(d0, d7, a1, vm);
    if (*d0 == 0) {
        dk::jump(at_Op2_Goto);
        return;
    }
    (*script)++;
    *a0 = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*a0);
}
