#include "game.h"

/* Obj_SetAttackBoxes and the body-box ops Op2_SetBodyBoxes, Op2_SetBodyBox, Op2_ClearBodyBoxes,
 * Op2_SetBodyBoxId ($003798-$003859). */

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

/* Obj_SetAttackBoxes's body, on a typed script, VM block and object */
static void rebuild_attack_boxes(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                                 uint32_t *d6, uint16_t *count, uint32_t *a1, const u16 **script, Vm2Block *vm,
                                 Vm2Obj *self, dk::Flags *ccr)
{
    vm->delay = 0;
    s16 *slot = &vm->attack[0];
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
        }
        HitBox *box = dk::handle<HitBox>((int16_t)(uint16_t)a0);
        box->on_hit = at_Native_NoOp;
        box->flags = 0;
        box->unk_3D = 0;
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
    uint16_t surplus = (uint16_t)(vm->attack_count - (uint16_t)*d6);
    vm->attack_count = (uint16_t)*d6;
    *ccr = dk::Flags::compare(surplus, 1, 2);
    ccr->x = ccr->c;
    *count = (uint16_t)(surplus - 1);
    if (!(*count & 0x8000u)) {
        do {
            ObjPool_Free((uint32_t)(int32_t)(int16_t)*slot);
            *slot++ = 0;
        } while (dbra(count));
        *ccr = dk::Flags::test(0, 2, ccr->x);   /* clr.w (a1)+ */
    }
    *a1 = dk::addr(slot);
}
/* Obj_SetAttackBoxes - rebuilds the object's attack-box set from `count` inline specs: reuses the
 * box already in each slot or allocates one, stamps the owner, its id and serial, the spec's kind
 * and rectangle, and the hit callback (default Native_NoOp) with its argument words; then frees
 * the boxes beyond the new count. A failed allocation skips the remaining specs (and frees from
 * the slot after the failed one: the original's off-by-one, kept).
 * In:  count (d7) = specs; script = specs: words kind, left, top, right, bottom, word n (0 = no
 *      callback, else n words: long callback + n-2 argument words); vm, self
 * Out: vm->delay = 0; vm->attack_count, vm->attack[]; d0-d6, d7, a1 as the asm leaves them
 *      (d1-d5 = the last spec's words, sign-extended); script past the specs; ccr = the
 *      condition codes of the last subq / clr.w
 * Evidence: clr.b $1(a4) / lea $C(a4),a1 / ObjPool_Alloc / movem.w (a2)+,d1-d5 / movem.w
 *           d0-d5,$4(a0) / move.l (a2)+,(a0)+ ... / ObjPool_Free / clr.w (a1)+; 153 runs
 */
void Obj_SetAttackBoxes(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                        uint32_t *d6, uint16_t *count, uint32_t *a1, uint32_t *a2, uint32_t a4, uint32_t a6,
                        dk::Flags *ccr)
{
    const u16 *script = at<const u16>(*a2);
    rebuild_attack_boxes(d0, d1, d2, d3, d4, d5, d6, count, a1, &script, at<Vm2Block>(a4), at<Vm2Obj>(a6), ccr);
    *a2 = dk::addr(script);
}

/* Obj_SetBodyBoxes (the next file) on a typed script, vm and object */
static void body_boxes(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                       uint32_t *d6, uint16_t *count, uint32_t *a1, const u16 **script, Vm2Block *vm, Vm2Obj *self)
{
    uint32_t a2 = dk::addr(*script);
    Obj_SetBodyBoxes(d0, d1, d2, d3, d4, d5, d6, count, a1, &a2, dk::addr(vm), dk::addr(self));
    *script = at<const u16>(a2);
}

/* Op2_SetBodyBoxes - a2 op: replaces the object's body boxes (the boxes it can be struck
 * through) with n boxes described inline (Obj_SetBodyBoxes).
 * In:  script = word n, then n box specs
 * Out: as Obj_SetBodyBoxes; NEXT after the specs
 * Evidence: move.w (a2)+,d7 / bsr.w Obj_SetBodyBoxes / NEXT; 3 runs; $180EA `381E 0002 0008 ...`
 */
void Op2_SetBodyBoxes(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                      uint32_t *d6, uint16_t *d7, uint32_t *a1, const u16 **script, Vm2Block *vm, Vm2Obj *self)
{
    *d7 = *(*script)++;
    body_boxes(d0, d1, d2, d3, d4, d5, d6, d7, a1, script, vm, self);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op2_SetBodyBox - a2 op: replaces the object's body boxes with the one box described inline.
 * In:  script = one box spec
 * Out: d7 = 1, then as Obj_SetBodyBoxes; handler = the next op's; NEXT after the spec
 * Evidence: moveq #1,d7 / bsr.w Obj_SetBodyBoxes / NEXT; 151 runs; $198D2 `3828 0008 FFDC FFBD
 *           0023 003A 0000` (kind 8 = the enemy list, no callback)
 */
void Op2_SetBodyBox(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                    uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, Vm2Block *vm,
                    Vm2Obj *self)
{
    uint16_t count = 1;   /* moveq: the upper word 0, and the box loop only counts the low word */
    body_boxes(d0, d1, d2, d3, d4, d5, d6, &count, a1, script, vm, self);
    *d7 = count;
    *a0 = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*a0);
}

/* Op2_ClearBodyBoxes - a2 op: removes all of the object's body boxes (unlinks them from their list
 * and frees them): the object can no longer be hit.
 * In:  -
 * Out: d7 = 0, then as Obj_SetBodyBoxes; handler = the next op's; NEXT
 * Evidence: moveq #0,d7 / bsr.w Obj_SetBodyBoxes / NEXT; 139 runs
 */
void Op2_ClearBodyBoxes(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                        uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, Vm2Block *vm,
                        Vm2Obj *self)
{
    uint16_t count = 0;
    body_boxes(d0, d1, d2, d3, d4, d5, d6, &count, a1, script, vm, self);
    *d7 = count;
    *a0 = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*a0);
}

/* Op2_SetBodyBoxId - a2 op: sets the id of every body box of the object (normally its owner id;
 * Hitbox_FindPlayerBoxOfId matches on it).
 * In:  script = word id; vm->body_count / body[]
 * Out: id (d0's low word); d7 = count - 1 then the loop's; a1 past the handles, a0 = the last box;
 *      handler = the next op's; NEXT (no boxes: VM2_Next)
 * Evidence: move.w (a2)+,d0 / move.w $2(a4),d7 / subq.w #1,d7 / bmi.w VM2_Next / lea $4(a4),a1 /
 *           movea.w (a1)+,a0 / move.w d0,$10(a0) / dbra; 68 runs; $4B7F6 `383C 0005`
 */
void Op2_SetBodyBoxId(uint16_t *id, uint16_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, Vm2Block *vm)
{
    *id = *(*script)++;
    *d7 = (uint16_t)(vm->body_count - 1);
    if (*d7 & 0x8000u) {
        dk::jump(at_VM2_Next);
        return;
    }
    const s16 *slot = &vm->body[0];
    do {
        *a0 = (uint32_t)(int32_t)(int16_t)*slot++;
        dk::handle<HitBox>((int16_t)(uint16_t)*a0)->id = *id;
    } while (dbra(d7));
    *a1 = dk::addr(slot);
    *a0 = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*a0);
}
