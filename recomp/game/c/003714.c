#include "game.h"

/* Op2_BranchTouchBox and its jump table ($003714-$003759): the overlap test's rectangle, then the
 * list the kind word picks (Op2_BranchTouchBox_Players / _Enemies do the search). */

/* the low word of a data register, the upper word kept (a .w operation) */
static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* neg.w on a register's low word */
static void neg_w(uint32_t *r) { set_low_word(r, (uint16_t)(0u - (uint16_t)*r)); }

/* exg.l */
static void exg(uint32_t *a, uint32_t *b)
{
    uint32_t t = *a;
    *a = *b;
    *b = t;
}

/* the sprite attributes' high byte: bit 3 = H flip, bit 4 = V flip */
enum { ATTR_HI_HFLIP = 1 << 3, ATTR_HI_VFLIP = 1 << 4 };

/* Op2_BranchTouchBox - a2 op: branches when any box on a collision list overlaps a rectangle
 * relative to the object (mirrored by its H/V flip bits), remembering that box in g_FoundBox; the
 * kind word picks the list through a jump table (0 / 4 = g_PlayerBoxList, 8 = g_EnemyBoxList).
 * In:  script = words kind, left, top, right, bottom (relative to self->x/y), word offset;
 *      spr->attr flips
 * Out: d1/d2 low words = x/y; d3-d7 = the operand words (sign-extended), d4-d7 then the world
 *      rectangle (low words); a0 = the offset word; then the kind's slot ($374A + kind)
 * Evidence: move.w $3E(a6),d1 / move.w $42(a6),d2 / movea.l a2,a0 / movem.w (a0)+,d3-d7 /
 *           btst #3,$38(a5) (neg.w d4 / neg.w d6 / exg d4,d6) / btst #4 (d5/d7) / add.w d1,d4 ...
 *           / jmp $374A(pc,d3.w); 148 runs; $132FE `3714 0000 FFFE FFFE 0002 0002 005A`
 */
void Op2_BranchTouchBox(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                        uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t a4,
                        const Vm2Sprite *spr, const Vm2Obj *self)
{
    set_low_word(d1, (uint16_t)self->x);
    set_low_word(d2, (uint16_t)self->y);
    const u16 *op = *script;
    uint32_t *words[5] = { d3, d4, d5, d6, d7 };
    for (int i = 0; i < 5; i++)
        *words[i] = (uint32_t)(int32_t)(int16_t)(uint16_t)op[i];   /* movem.w: sign-extended */
    *a0 = dk::addr(op + 5);
    uint8_t attr_hi = (uint8_t)(spr->attr >> 8);
    if (attr_hi & ATTR_HI_HFLIP) {
        neg_w(d4);
        neg_w(d6);
        exg(d4, d6);
    }
    if (attr_hi & ATTR_HI_VFLIP) {
        neg_w(d5);
        neg_w(d7);
        exg(d5, d7);
    }
    set_low_word(d4, (uint16_t)(*d4 + *d1));
    set_low_word(d6, (uint16_t)(*d6 + *d1));
    set_low_word(d5, (uint16_t)(*d5 + *d2));
    set_low_word(d7, (uint16_t)(*d7 + *d2));
    dk::jump(at_Op2_BranchTouchBox_Kind0 + (uint32_t)(int32_t)(int16_t)(uint16_t)*d3);
}

/* Op2_BranchTouchBox_Kind0 - the jump table's kind-0 slot: the player list.
 * In:  the rectangle in d4-d7; a0 = the op's offset word
 * Out: bra.w Op2_BranchTouchBox_Players
 * Evidence: the jmp $374A(pc,d3.w) at $3746 with d3 = 0: 751,562 call-site log entries
 */
void Op2_BranchTouchBox_Kind0(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                              uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t a4,
                              uint32_t a5, uint32_t a6)
{
    dk::jump(at_Op2_BranchTouchBox_Players);
}

/* Op2_BranchTouchBox_Kind4 - the jump table's kind-4 slot: also the player list.
 * In:  as Op2_BranchTouchBox_Kind0
 * Out: bra.w Op2_BranchTouchBox_Players
 * Evidence: the jmp at $3746 with d3 = 4: 707,192 call-site log entries
 */
void Op2_BranchTouchBox_Kind4(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                              uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t a4,
                              uint32_t a5, uint32_t a6)
{
    dk::jump(at_Op2_BranchTouchBox_Players);
}

/* Op2_BranchTouchBox_Kind8 - the jump table's kind-8 slot: the enemy list.
 * In:  as Op2_BranchTouchBox_Kind0
 * Out: bra.w Op2_BranchTouchBox_Enemies
 * Evidence: the jmp at $3746 with d3 = 8: 18,686 call-site log entries
 */
void Op2_BranchTouchBox_Kind8(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                              uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t a4,
                              uint32_t a5, uint32_t a6)
{
    dk::jump(at_Op2_BranchTouchBox_Enemies);
}
