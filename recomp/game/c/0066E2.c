#include "game.h"

/* Op4_BodyBox / Op4_SwitchHeading / Op4_BranchScreenClear ($0066E2-$00676B). */

static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op4_BodyBox - a4 op: keeps a shootable box on the object: creates it on first use (a pool block
 * on the enemy box list, with the id and the energy, the hit callback Native_NoOp) and recentres
 * it on d4 / d5 every run; branches once the box's energy is used up. With the pool empty it
 * skips its operands.
 * In:  script = word branch offset (energy <= 0), word box id, word energy, word half-width, word
 *      half-height; d4 / d5 = the drawn x / y words; self = a6; a1 passed to Hitbox_LinkEnemyList
 * Out: self->box; the box rectangle = d4 / d5 -+ w / h; d0-d3 = left, top, right, bottom (word
 *      ops: d0's upper word kept); a0 = the box, then the handler; the condition codes of the last
 *      add.w (or the tst / ObjPool_Alloc's); NEXT or the branch (Op4_Goto)
 * Evidence: move.w $14(a6),d0 / beq / tst.w $1E(a0) / ble.w Op4_Goto / addq.l #6,a4 / neg.w /
 *           add.w / movem.w d0-d3,$12(a0); bsr ObjPool_Alloc / bsr Hitbox_LinkEnemyList /
 *           move.l #$82C,$30(a0) / clr.w $3C(a0) / clr.w $20(a0); script uses $1B558 (`66E2 001E
 *           000C 0010 000A 000A`), $359E0; 23 corpus runs
 */
void Op4_BodyBox(uint32_t *d0, uint16_t *d1, uint16_t *d2, uint16_t *d3, uint16_t d4, uint16_t d5, uint32_t *a0,
                 uint32_t a1, const u16 **script, Vm4Obj *self, dk::Flags *ccr)
{
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)self->box;
    PTR(HitBox) box;
    if (self->box != 0) {
        *a0 = (uint32_t)(int32_t)self->box;
        box.v = *a0;
        if (box->energy <= 0) {
            *ccr = dk::Flags::test((uint16_t)box->energy, 2);
            dk::jump(at_Op4_Goto);
            return;
        }
        *script += 3;   /* the offset, id and energy */
    } else {
        ObjPool_Alloc(d0, a0);
        if (*d0 != 0) {
            *ccr = dk::Flags::test(*d0, 4);   /* moveq #-1,d0 */
            *script += 5;
            next(a0, script);
            return;
        }
        dk::Flags link = *ccr;   /* Hitbox_LinkEnemyList's flags: the add.w below replaces them */
        Hitbox_LinkEnemyList(*a0, a1, &link);
        box.v = *a0;
        box->on_hit = at_Native_NoOp;
        box->flags = 0;   /* clr.w $3C(a0) */
        box->unk_3D = 0;
        box->damage = 0;
        self->box = (int16_t)(uint16_t)*a0;
        *script += 1;     /* the branch offset */
        box->id = *(*script)++;
        box->energy = (int16_t)(uint16_t)*(*script)++;
    }
    uint16_t w = *(*script)++;
    uint16_t h = *(*script)++;
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)(-w + d4);
    *d1 = (uint16_t)(-h + d5);
    *d2 = (uint16_t)(w + d4);
    uint16_t sum = (uint16_t)(h + d5);
    bool carry = sum < h;
    *ccr = dk::Flags{ carry, (sum & 0x8000) != 0, sum == 0, ((~(h ^ d5)) & (h ^ sum) & 0x8000) != 0, carry };
    *d3 = sum;
    box->left = (int16_t)(uint16_t)*d0;
    box->top = (int16_t)*d1;
    box->right = (int16_t)*d2;
    box->bottom = (int16_t)*d3;
    next(a0, script);
}

/* Op4_SwitchHeading - a4 op: branches to one of n offsets by the object's heading: index =
 * ((heading + rotation) & $3FE) * n / $400.
 * In:  script = word rotation, word n, n word offsets (each relative to itself); self = a6
 * Out: d0 = the doubled index (upper word: the product's >> 8); the chosen branch
 * Evidence: move.w $2A(a6),d0 / add.w (a4)+,d0 / andi.w #$3FE / mulu.w (a4)+,d0 / lsr.l #8 /
 *           lsr.w #2 / add.w d0,d0 / adda.w d0,a4 / adda.w (a4),a4; script uses $14754 (`6744 0140
 *           0008` + 8 offsets), $28C46; 21 corpus runs
 */
void Op4_SwitchHeading(uint32_t *d0, const u16 **script, const Vm4Obj *self)
{
    uint16_t h = (uint16_t)((uint16_t)self->heading + *(*script)++) & 0x3FE;
    *d0 = ((uint32_t)h * *(*script)++) >> 8;
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)(((uint16_t)*d0 >> 2) * 2);
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)*d0);
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    uint32_t handler;
    next(&handler, script);
}

/* Op4_BranchScreenClear - a4 op: branches while a screen-clear pause runs (g_ScreenClearTimer
 * nonzero); otherwise continues past the offset.
 * In:  script = word branch offset
 * Out: the branch (Op4_Goto) or NEXT after the offset
 * Evidence: tst.w (g_ScreenClearTimer).w / bne.w Op4_Goto / addq.l #2,a4 / NEXT; script uses $1B540
 *           (`675E 0036`), $359D0, $42D14; 17 corpus runs
 */
void Op4_BranchScreenClear(uint32_t *handler, const u16 **script)
{
    if (g_ScreenClearTimer != 0) {
        dk::jump(at_Op4_Goto);
        return;
    }
    (*script)++;
    next(handler, script);
}
