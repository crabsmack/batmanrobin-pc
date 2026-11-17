#include "game.h"

/* a 68000 address held in a register, as a pointer (PTR(T): the 68000 value kept) */
template <class T> static T *ptr(uint32_t a)
{
    PTR(T) p;
    p.v = a;
    return p;
}

/* $008CCA-$008DB5: the id scan of g_PlayerBoxList (Hitbox_FindNextBoxOfId and its loop),
 * Hitbox_DamageAllEnemies, and the Spawn_At* native callbacks: a script's Op2_CallNative names one,
 * followed by `dc.l routine` (and for some an offset); the callback works out a position in d4/d5
 * and calls the routine (an enemy spawner such as `lea script,a2 / jmp Obj_SpawnEnemy`) with the
 * script pointer in a1, a3-a6 saved around it, then resumes the script where a1 ends. */

/* ---- the id scan ---- */

/* the rectangle and id the scan tests against (d3-d7) */
struct BoxQuery_8CCA {
    uint16_t id;
    int16_t left, top, right, bottom;
};

/* the id compare, then the four rectangle compares, in the asm's order */
static bool matches(const HitBox *box, const BoxQuery_8CCA &q)
{
    if (q.id != (uint16_t)box->id) return false;   /* cmp.w $10(a2),d3 / bne */
    if (q.left > box->right) return false;         /* cmp.w $16(a2),d4 / bgt */
    if (q.right < box->left) return false;         /* cmp.w $12(a2),d6 / blt */
    if (q.top > box->bottom) return false;         /* cmp.w $18(a2),d5 / bgt */
    if (q.bottom < box->top) return false;         /* cmp.w $14(a2),d7 / blt */
    return true;
}

/* the scan from box *a2 (tested first when test_first): d0 = -1 and N set on a match; at the tail
   sentinel Z set (tst.w (a2) of its 0 link) */
static void scan(uint32_t *d0, const BoxQuery_8CCA &q, uint32_t *a2, bool test_first, dk::Flags *ccr)
{
    const HitBox *box = ptr<const HitBox>(*a2);
    if (!test_first || !matches(box, q)) {
        for (;;) {
            *a2 = (uint32_t)(int32_t)box->next;   /* movea.w (a2),a2 */
            box = ptr<const HitBox>(*a2);
            if (box->next == 0) {
                *ccr = dk::Flags::test(0, 2, ccr->x);
                return;
            }
            if (matches(box, q))
                break;
        }
    }
    *d0 = 0xFFFFFFFFu;
    *ccr = dk::Flags::test(*d0, 4, ccr->x);
}

/* Hitbox_FindNextBoxOfId - continues a Hitbox_FindPlayerBoxOfId scan from the box after a2.
 * In:  a2 = the last box found; d3 = the id; d4 left, d5 top, d6 right, d7 bottom
 * Out: Z clear, d0 = -1, a2 = the next matching box; Z set at the end (a2 = the tail sentinel)
 * Evidence: movea.w (a2),a2 / tst.w (a2) / beq.s Hitbox_ScanOfIdDone, then Hitbox_TestBoxOfId's
 *           compares (each miss branches back here)
 */
void Hitbox_FindNextBoxOfId(uint32_t *d0, uint16_t d3, uint16_t d4, uint16_t d5, uint16_t d6, uint16_t d7,
                            uint32_t *a2, dk::Flags *ccr)
{
    BoxQuery_8CCA q = { d3, (int16_t)d4, (int16_t)d5, (int16_t)d6, (int16_t)d7 };
    scan(d0, q, a2, false, ccr);
}

/* Hitbox_TestBoxOfId - the id-and-overlap test the id scan starts with: box a2's id against d3,
 * then its rectangle against d4-d7; a miss goes on with the next box.
 * In:  a2 = a box (not the tail sentinel); d3 = the id; d4 left, d5 top, d6 right, d7 bottom
 * Out: Z clear, d0 = -1, a2 = the first matching box from a2 on; Z set at the end of the list
 * Evidence: cmp.w $10(a2),d3 / bne / cmp.w $16(a2),d4 / bgt ... / moveq #-1,d0; bne.s here from
 *           Hitbox_FindPlayerBoxOfId. Prototype: a2 an output (it was a clobber), as
 *           Hitbox_FindNextBoxOfId's
 */
void Hitbox_TestBoxOfId(uint32_t *d0, uint16_t d3, uint16_t d4, uint16_t d5, uint16_t d6, uint16_t d7,
                        uint32_t *a2, dk::Flags *ccr)
{
    BoxQuery_8CCA q = { d3, (int16_t)d4, (int16_t)d5, (int16_t)d6, (int16_t)d7 };
    scan(d0, q, a2, true, ccr);
}

/* Hitbox_ScanOfIdDone - the rts the id scans end on (a function of its own in the generated code).
 * In:  none
 * Out: none (the condition codes stay as the scan left them)
 * Evidence: beq.s Hitbox_ScanOfIdDone in Hitbox_FindNextBoxOfId; moveq #-1,d0 falls into it
 */
void Hitbox_ScanOfIdDone(void)
{
}

/* Hitbox_DamageAllEnemies - every box on g_EnemyBoxList is struck: marked struck by attacker d1,
 * damage d0 added, the knockback's high word cleared (a hit that doesn't push).
 * In:  d0 = damage; d1 = the attacker's id
 * Out: each box: BOX_STRUCK set, knockback high word 0, damage + d0, struck_by = d1; ccr = the
 *      final tst.w of the tail sentinel's 0 link (N V C clear)
 * Evidence: movea.w g_EnemyBoxList,a0 / tst.w (a0) / beq / bset #0,$3C(a0) / clr.w $22(a0) /
 *           add.w d0,$20(a0) / move.w d1,$1A(a0) / movea.w (a0),a0 / bra; callers Hitbox_WipeEnemies
 *           and $1375E (the bomb), both d0 = $200
 */
void Hitbox_DamageAllEnemies(uint16_t d0, uint16_t d1, dk::Flags *ccr)
{
    HitBox *box = dk::handle<HitBox>((int16_t)g_EnemyBoxList);
    while (box->next != 0) {
        box->flags |= BOX_STRUCK;
        box->knockback = (int32_t)((uint32_t)box->knockback & 0xFFFFu);   /* clr.w: the high word */
        box->damage += d0;
        box->struck_by = d1;
        box = dk::handle<HitBox>(box->next);
    }
    *ccr = dk::Flags::test(0, 2, ccr->x);
}

/* ---- the Spawn_At* native callbacks ---- */

/* the high word of a 16.16 camera coordinate (move.w (g_CameraX).w reads the integer part) */
static uint16_t camera_px(uint32_t pos16_16)
{
    return (uint16_t)(pos16_16 >> 16);
}

static void set_low_word(uint32_t *r, uint16_t v)
{
    *r = (*r & 0xFFFF0000u) | v;
}

/* the shared tail: a1 = the script after the routine's address, a3-a6 pushed (movem.l a3-a6,-(a7):
   the same stack image under the routine), jsr (a0), then a2 = a1 as the routine left it and
   a3-a6 popped back (the slot keeps them: they are no outputs). The routine's condition codes are
   the callback's. */
static void call_routine(uint32_t routine, uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7,
                         uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    *a1 = *a2;
    dk::push_long(a6);
    dk::push_long(a5);
    dk::push_long(a4);
    dk::push_long(a3);
    uint32_t a0 = routine, r2 = *a2, r3 = a3, r4 = a4, r5 = a5, r6 = a6;
    SpawnRoutineFn(dk::code(routine), d0, d1, d4, d5, d7, &a0, a1, &r2, &r3, &r4, &r5, &r6);
    *a2 = *a1;
    for (int i = 0; i < 4; i++)
        (void)dk::pop_long();
}

/* the script's dc.l routine (movea.l (a2)+,a0) */
static uint32_t take_long(uint32_t *a2)
{
    uint32_t v = *ptr<const u32>(*a2);
    *a2 += 4;
    return v;
}

static uint16_t take_word(uint32_t *a2)
{
    uint16_t v = *ptr<const u16>(*a2);
    *a2 += 2;
    return v;
}

/* Spawn_AtScreenCentre - native callback: calls the script's routine at the screen centre (camera
 * + ($A0, $70)).
 * In:  a2 = the script: dc.l routine, then the routine's arguments; a3-a6 passed to the routine
 * Out: d4/d5 = the position (low words); d0, d1, d7 as the routine leaves them; a1 = a2 = past
 *      what the routine consumed; a3-a6 kept
 * Evidence: movea.l (a2)+,a0 / move.w g_CameraX,d4 / move.w g_CameraY,d5 / addi.w #$A0,d4 /
 *           addi.w #$70,d5 / movea.l a2,a1 / movem.l a3-a6,-(a7) / jsr (a0) / movea.l a1,a2 /
 *           movem.l (a7)+,a3-a6; 26 script uses after Op2_CallNative ($46F18...).
 *           Prototype: a3/a5 inputs (pushed), d1/d4/d5 widened to longs (the routine gets them whole)
 */
void Spawn_AtScreenCentre(uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1,
                          uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    uint32_t routine = take_long(a2);
    set_low_word(d4, (uint16_t)(camera_px(g_CameraX) + 0xA0));
    set_low_word(d5, (uint16_t)(camera_px(g_CameraY) + 0x70));
    call_routine(routine, d0, d1, d4, d5, d7, a1, a2, a3, a4, a5, a6);
}

/* Spawn_AtCameraOfs - native callback: calls the script's routine at the camera position plus the
 * offset words after it.
 * In:  a2 = the script: dc.l routine, dc.w dx, dy, then the routine's arguments
 * Out: d4/d5 = camera + (dx, dy) (low words); the rest as Spawn_AtScreenCentre
 * Evidence: movea.l (a2)+,a0 / move.w (a2)+,d4 / move.w (a2)+,d5 / add.w g_CameraX,d4 /
 *           add.w g_CameraY,d5 / the shared call; script uses $1E7F4, $1E956, $3CC70
 */
void Spawn_AtCameraOfs(uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1,
                       uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    uint32_t routine = take_long(a2);
    uint16_t dx = take_word(a2);
    uint16_t placed = ws_place_x(dx);   /* native: placed for the wide view's edge (the gameplay mode, R20) */
    WS_NOTE_PLACE(WS_PLACE_CAMERA_OFS, (int16_t)dx, (int16_t)(uint16_t)(placed - dx), 0, routine);
    set_low_word(d4, placed);
    set_low_word(d5, take_word(a2));
    set_low_word(d4, (uint16_t)(*d4 + camera_px(g_CameraX)));
    set_low_word(d5, (uint16_t)(*d5 + camera_px(g_CameraY)));
    call_routine(routine, d0, d1, d4, d5, d7, a1, a2, a3, a4, a5, a6);
}

/* Spawn_CallAtD4D5 (a static helper: only this file's
 * entries reach it, by bra.s and by falling in) - the shared tail of Spawn_AtAnchor / AtDrawPos / AtOwnPos: calls the script's
 * routine with the position already in d4/d5.
 * In:  d4/d5 = the position; a2 = the script at the dc.l routine; a3-a6 passed to the routine
 * Out: a1 = a2 = past what the routine consumed; d0, d1, d7 as it leaves them; a3-a6 kept
 * Evidence: movea.l (a2)+,a0 / movea.l a2,a1 / movem.l a3-a6,-(a7) / jsr (a0) / movea.l a1,a2 /
 *           movem.l (a7)+,a3-a6 / rts
 */
static void spawn_call_at_d4d5(uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7,
                               uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    uint32_t routine = take_long(a2);
    call_routine(routine, d0, d1, d4, d5, d7, a1, a2, a3, a4, a5, a6);
}

/* Spawn_AtAnchor - native callback: calls the script's routine at the object's saved position
 * (+$46/+$4A, the copy Op2_SavePos makes).
 * In:  a6 = the object; a2 = the script: dc.l routine + its arguments
 * Out: d4/d5 = the saved position's integer parts; then Spawn_CallAtD4D5
 * Evidence: move.w $46(a6),d4 / move.w $4A(a6),d5 / bra.s Spawn_CallAtD4D5; 22 script uses in
 *           $38886-$38A58
 */
void Spawn_AtAnchor(uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1,
                    uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    const Vm2Obj *self = ptr<const Vm2Obj>(a6);
    set_low_word(d4, (uint16_t)((uint32_t)self->saved_x >> 16));
    set_low_word(d5, (uint16_t)((uint32_t)self->saved_y >> 16));
    spawn_call_at_d4d5(d0, d1, d4, d5, d7, a1, a2, a3, a4, a5, a6);
}

/* Spawn_AtDrawPos - native callback: calls the script's routine at the object's drawn position
 * (+$4E/+$50).
 * In:  a6 = the object; a2 = the script: dc.l routine + its arguments
 * Out: d4/d5 = the drawn position; then Spawn_CallAtD4D5
 * Evidence: move.w $4E(a6),d4 / move.w $50(a6),d5 / bra.s Spawn_CallAtD4D5; 11 script uses at
 *           $38918-$389E6
 */
void Spawn_AtDrawPos(uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1,
                     uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    const Vm2Obj *self = ptr<const Vm2Obj>(a6);
    set_low_word(d4, (uint16_t)self->draw_x);
    set_low_word(d5, (uint16_t)self->draw_y);
    spawn_call_at_d4d5(d0, d1, d4, d5, d7, a1, a2, a3, a4, a5, a6);
}

/* Spawn_AtOwnPos - native callback: calls the script's routine at the object's position.
 * In:  a6 = the object; a2 = the script: dc.l routine + its arguments
 * Out: d4/d5 = x/y (+$3E/+$42); then Spawn_CallAtD4D5 (it falls into it)
 * Evidence: move.w $3E(a6),d4 / move.w $42(a6),d5; 21 script uses ($299AE, $2F12A, $3FB48...)
 */
void Spawn_AtOwnPos(uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1,
                    uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    const Vm2Obj *self = ptr<const Vm2Obj>(a6);
    set_low_word(d4, (uint16_t)self->x);
    set_low_word(d5, (uint16_t)self->y);
    spawn_call_at_d4d5(d0, d1, d4, d5, d7, a1, a2, a3, a4, a5, a6);
}

/* Spawn_AtOwnOfs - native callback: calls the script's routine at the object's position plus an
 * offset mirrored by the sprite's flips (H flip negates dx, V flip negates dy).
 * In:  a6 = the object; a5 = its sprite block (attr bits 11/12 = H/V flip); a2 = the script:
 *      dc.l routine, dc.w dx, dy, the routine's arguments
 * Out: d4/d5 = x + dx, y + dy (low words); d0 = the routine's (the dy before it); the rest as
 *      Spawn_CallAtD4D5
 * Evidence: movea.l (a2)+,a0 / move.w $3E(a6),d4 / move.w $42(a6),d5 / move.w (a2)+,d0 /
 *           btst #3,$38(a5) / neg.w d0 / add.w d0,d4 / move.w (a2)+,d0 / btst #4,$38(a5) / neg.w d0
 *           / add.w d0,d5, then the shared call; 86 script uses ($29DBE, $2CD9E...)
 */
void Spawn_AtOwnOfs(uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1,
                    uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    enum { SPAWN_ATTR_HI_HFLIP = 1 << 3, SPAWN_ATTR_HI_VFLIP = 1 << 4 };   /* attr's high byte */
    const Vm2Obj *self = ptr<const Vm2Obj>(a6);
    const Vm2Sprite *spr = ptr<const Vm2Sprite>(a5);
    uint32_t routine = take_long(a2);
    set_low_word(d4, (uint16_t)self->x);
    set_low_word(d5, (uint16_t)self->y);
    set_low_word(d0, take_word(a2));
    if ((uint8_t)(spr->attr >> 8) & SPAWN_ATTR_HI_HFLIP)
        set_low_word(d0, (uint16_t)-(uint16_t)*d0);
    set_low_word(d4, (uint16_t)(*d4 + *d0));
    set_low_word(d0, take_word(a2));
    if ((uint8_t)(spr->attr >> 8) & SPAWN_ATTR_HI_VFLIP)
        set_low_word(d0, (uint16_t)-(uint16_t)*d0);
    set_low_word(d5, (uint16_t)(*d5 + *d0));
    call_routine(routine, d0, d1, d4, d5, d7, a1, a2, a3, a4, a5, a6);
}
