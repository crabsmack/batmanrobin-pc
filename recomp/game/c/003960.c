#include "game.h"

/* Obj_TestStruck and its struck exit Obj_SetStruckBox ($003960-$003989). */

/* Obj_TestStruck - tests and clears the struck bit of the object's body boxes in order, stopping
 * at the first one that was set (empty slots skipped).
 * In:  vm->body_count / body[]
 * Out: struck: g_StruckBox = the box, d0 = 1; else d0 = 0 (Z from d0); d7 = the loop counter's
 *      low word; a1 past the last handle read
 * Evidence: lea $4(a4),a1 / move.w $2(a4),d7 / subq.w #1,d7 / bmi / move.w (a1)+,d0 /
 *           movea.w d0,a0 / bclr #0,$3C(a0) / bne $3982 / dbra / moveq #0,d0; 151 runs
 */
void Obj_TestStruck(uint32_t *d0, uint16_t *d7, uint32_t *a1, Vm2Block *vm)
{
    const s16 *slot = &vm->body[0];
    *d7 = (uint16_t)(vm->body_count - 1);
    if (!(*d7 & 0x8000u)) {
        do {
            uint16_t h = (uint16_t)*slot++;
            *d0 = (*d0 & 0xFFFF0000u) | h;
            if (h == 0)
                continue;
            HitBox *box = dk::handle<HitBox>((int16_t)h);
            bool struck = box->flags & BOX_STRUCK;
            box->flags = (uint8_t)(box->flags & ~BOX_STRUCK);
            if (struck) {
                *a1 = dk::addr(slot);
                *d0 = Obj_SetStruckBox(box);
                return;
            }
        } while (--*d7 != 0xFFFFu);
    }
    *a1 = dk::addr(slot);
    *d0 = 0;
}

/* Obj_SetStruckBox - the struck exit of Obj_TestStruck and Obj_TestBoxStruck: remembers the box
 * and returns 1.
 * In:  box (a0) = the struck box
 * Out: g_StruckBox = it; d0 = 1 (Z clear)
 * Evidence: move.l a0,(g_StruckBox).w / moveq #1,d0 / rts; the bne targets after bclr #0,$3C(a0)
 */
uint32_t Obj_SetStruckBox(HitBox *box)
{
    g_StruckBox = dk::addr(box);
    return 1;
}
