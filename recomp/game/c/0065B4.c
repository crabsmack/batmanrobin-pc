#include "game.h"

/* $0065B4-$00664D: the a4 touch tests: a box of half-width w and half-height h around the drawn
 * point (d4 / d5) against the players' or the enemies' collision list, branching on an overlap;
 * Op4_HitPlayers calls a native hit routine for every player box it overlaps. The "OnPhase"
 * variants run only on the object's frame phase (g_FrameParity == hit_phase). */

static void next(uint32_t *handler, uint32_t *a4)
{
    PTR(const u16) script;
    script.v = *a4;
    *handler = dk::code((int16_t)(uint16_t)*script);
    *a4 += 2;
    dk::jump(*handler);
}

/* the word operand at a4, a4 past it */
static uint16_t take_word(uint32_t *a4)
{
    PTR(const u16) script;
    script.v = *a4;
    *a4 += 2;
    return *script;
}

/* the box searches return a2 = the box found, or the list's tail sentinel (next link 0) */
static bool found_box(uint32_t a2)
{
    PTR(const HitBox) box;
    box.v = a2;
    return box->next != 0;
}

/* the test rectangle: d2 / d3 keep the point, d4-d7 = left, top, right, bottom (word ops) */
template <class R>
static void touch_rect(R *d2, R *d3, R *d4, R *d5, R *d6, R *d7, uint32_t *a4)
{
    auto set = [](R *r, uint16_t v) { *r = (R)((*r & ~(R)0xFFFF) | v); };
    set(d2, (uint16_t)*d4);
    set(d3, (uint16_t)*d5);
    set(d6, take_word(a4));
    set(d7, take_word(a4));
    set(d4, (uint16_t)(*d4 - *d6));
    set(d6, (uint16_t)(*d6 + *d2));
    set(d5, (uint16_t)(*d5 - *d7));
    set(d7, (uint16_t)(*d7 + *d3));
}

/* Op4_BranchTouchPlayer - a4 op: branches when a player box overlaps the box d4 +- w by d5 +- h
 * (Hitbox_FindPlayerBox); otherwise restores d4 / d5 and continues past the offset.
 * In:  a4 = script: word half-width, word half-height, word branch offset; d4 / d5 = the drawn x /
 *      y words
 * Out: d2 / d3 = the point; d4-d7 = the rectangle (d4 / d5 the point again when nothing
 *      touched); a2 = the box found (or the tail sentinel); the branch (Op4_Goto) or NEXT
 * Evidence: move.w d4,d2 / move.w d5,d3 / move.w (a4)+,d6 / move.w (a4)+,d7 / sub.w / add.w /
 *           bsr.w Hitbox_FindPlayerBox / bne.w Op4_Goto / move.w d2,d4 / move.w d3,d5; script uses
 *           $1323C (`65B4 0004 0004 0004`, health pickup: touched -> collect), $1ABA2; 86 corpus runs
 */
void Op4_BranchTouchPlayer(uint32_t *d0, uint16_t *d2, uint16_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6,
                           uint16_t *d7, uint32_t *a0, uint32_t *a2, uint32_t *a4)
{
    touch_rect(d2, d3, d4, d5, d6, d7, a4);
    Hitbox_FindPlayerBox(d0, *d4, *d5, *d6, *d7, a2);
    if (found_box(*a2)) {
        dk::jump(at_Op4_Goto);
        return;
    }
    *d4 = *d2;
    *d5 = *d3;
    *a4 += 2;
    next(a0, a4);
}

/* Op4_HitPlayersOnPhase - a4 op: Op4_HitPlayers on the object's own frame phase only; on the
 * other frames skips its 10 operand bytes.
 * In:  a4 = script: as Op4_HitPlayers; a6 = the object (hit_phase from Op4_TakeHitPhaseB)
 * Out: d0's low byte = g_FrameParity; then Op4_HitPlayers', or NEXT past the operands
 * Evidence: move.b (g_FrameParity).w,d0 / cmp.b $18(a6),d0 / beq.s Op4_HitPlayers / lea $A(a4),a4;
 *           enemy bullet scripts $42984 (`65D6 0005 0005 0004 29BC 0004`), $42F6A, $46B68; 10
 *           corpus runs
 */
void Op4_HitPlayersOnPhase(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                           uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                           uint32_t *a4, uint32_t a5, uint32_t *a6)
{
    PTR(const Vm4Obj) self;
    self.v = *a6;
    *d0 = (*d0 & 0xFFFFFF00u) | g_FrameParity;
    if ((uint8_t)*d0 == self->hit_phase) {
        Op4_HitPlayers(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
        return;
    }
    *a4 += 10;
    next(a0, a4);
}

/* Op4_HitPlayers - a4 op: calls a native hit routine (a3) for every player box overlapping d4 +- w
 * by d5 +- h (a2 = the box), then branches; with no overlap continues past the offset.
 * In:  a4 = script: word half-width, word half-height, long routine, word branch offset; d4 / d5
 *      = the drawn x / y words
 * Out: a3 = the routine; d2 / d3 = the point, d4 / d5 restored to it; the routines' registers;
 *      the branch or NEXT after the offset
 * Evidence: the Op4_BranchTouchPlayer rectangle / movea.l (a4)+,a3 / bsr.w Hitbox_FindPlayerBox /
 *           bne / jsr (a3) / bsr.w Hitbox_FindNextBox / bne loop / adda.w (a4),a4; script uses
 *           $15066 (`65E8 0003 0003 0001 06A8 000C`: routine $106A8), $150A4; 103 corpus runs
 */
void Op4_HitPlayers(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                    uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                    uint32_t *a4, uint32_t a5, uint32_t *a6)
{
    touch_rect(d2, d3, d4, d5, d6, d7, a4);
    *a3 = (uint32_t)take_word(a4) << 16;
    *a3 |= take_word(a4);
    Hitbox_FindPlayerBox(d0, (uint16_t)*d4, (uint16_t)*d5, (uint16_t)*d6, (uint16_t)*d7, a2);
    if (found_box(*a2)) {
        dk::Flags found{};
        do {
            Op4HitFn(dk::code(*a3), d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, &a5, a6);
            PTR(HitBox) box;
            box.v = *a2;
            HitBox *b = box.get();
            Hitbox_FindNextBox(d0, *d4, *d5, *d6, *d7, &b, &found);
            *a2 = dk::addr(b);
        } while (!found.z);
        *d4 = (*d4 & 0xFFFF0000u) | (uint16_t)*d2;
        *d5 = (*d5 & 0xFFFF0000u) | (uint16_t)*d3;
        PTR(const u16) offset;
        offset.v = *a4;
        *a4 += (uint32_t)(int32_t)(int16_t)(uint16_t)*offset;
        next(a0, a4);
        return;
    }
    *d4 = (*d4 & 0xFFFF0000u) | (uint16_t)*d2;
    *d5 = (*d5 & 0xFFFF0000u) | (uint16_t)*d3;
    *a4 += 2;
    next(a0, a4);
}

/* Op4_BranchTouchEnemyOnPhase - a4 op: on the object's own frame phase branches when an enemy box
 * overlaps d4 +- w by d5 +- h (Hitbox_FindEnemyBox); otherwise (or on the other frames) continues
 * past the operands.
 * In:  a4 = script: word half-width, word half-height, word branch offset; d4 / d5; a6 = the
 *      object (hit_phase)
 * Out: d0's low byte = g_FrameParity; as Op4_BranchTouchPlayer (a2 = the box)
 * Evidence: move.b (g_FrameParity).w,d0 / cmp.b $18(a6),d0 / beq / addq.l #6,a4; the
 *           Op4_BranchTouchPlayer code with bsr.w Hitbox_FindEnemyBox; player shot scripts $505A0
 *           (`661C 0008 0008 0048 67D2`), $508DC; 150 corpus runs
 */
void Op4_BranchTouchEnemyOnPhase(uint32_t *d0, uint16_t *d2, uint16_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6,
                                 uint16_t *d7, uint32_t *a0, uint32_t *a2, uint32_t *a4, const Vm4Obj *self)
{
    *d0 = (*d0 & 0xFFFFFF00u) | g_FrameParity;
    if ((uint8_t)*d0 != self->hit_phase) {
        *a4 += 6;
        next(a0, a4);
        return;
    }
    touch_rect(d2, d3, d4, d5, d6, d7, a4);
    Hitbox_FindEnemyBox(d0, *d4, *d5, *d6, *d7, a2);
    if (found_box(*a2)) {
        dk::jump(at_Op4_Goto);
        return;
    }
    *d4 = *d2;
    *d5 = *d3;
    *a4 += 2;
    next(a0, a4);
}
