#include "game.h"

/* The two halves of Mot_Atan2 with a nonzero dx ($014EE4-$014F77). Each octant's heading is its
 * axis heading plus or minus (minor << 7) / major: 0-$80 across the octant. The comparisons are
 * the asm's cmp.w / bmi (the sign of the 16-bit difference, not a signed less-than), and the
 * divisions divu.w, which leave the register unchanged when the quotient overflows 16 bits. */

/* the low word of a data register, the upper word kept (a .w move) */
static inline void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* cmp.w b,a / bmi: the 16-bit a - b is negative */
static inline bool minus(uint32_t a, uint32_t b) { return ((uint16_t)(a - b) & 0x8000u) != 0; }

/* ext.l a / lsl.l #7,a / divu.w b,a: the octant ratio, the quotient in a's low word (and the
   remainder in its upper word); a quotient over $FFFF leaves a as the shifted dividend */
static uint16_t ratio(uint32_t *a, uint32_t b)
{
    uint32_t v = (uint32_t)(int32_t)(int16_t)*a << 7;
    uint16_t divisor = (uint16_t)b;
    uint32_t q = v / divisor;
    if (q <= 0xFFFFu)
        v = ((v % divisor) << 16) | q;
    *a = v;
    return (uint16_t)v;
}

/* Mot_Atan2_RightHalf - the dx > 0 half of Mot_Atan2: headings $300-$3FF and 0-$100.
 * In:  d0 = dx > 0, d1 = dy (low words)
 * Out: d2 = the heading (its low word; 0 a long for dy = 0); d0 or d1 = the divu result, d1
 *      negated first when dy < 0
 * Evidence: tst.w d1 / bmi / beq Mot_Atan2_Zero / cmp.w d1,d0 / bmi; then d2 = ratio, $100 - ratio,
 *           $400 - ratio or $300 + ratio; reached by bne from Mot_Atan2
 */
void Mot_Atan2_RightHalf(uint32_t *d0, uint32_t *d1, uint32_t *d2)
{
    if ((int16_t)*d1 >= 0) {
        if ((uint16_t)*d1 == 0) {
            dk::jump(at_Mot_Atan2_Zero);
            return;
        }
        if (!minus(*d0, *d1))
            set_low_word(d2, ratio(d1, *d0));                          /* below the +x axis */
        else
            set_low_word(d2, (uint16_t)(0x100 - ratio(d0, *d1)));      /* toward +y */
        return;
    }
    set_low_word(d1, (uint16_t)-(int16_t)*d1);
    if (!minus(*d0, *d1))
        set_low_word(d2, (uint16_t)(0x400 - ratio(d1, *d0)));          /* above the +x axis */
    else
        set_low_word(d2, (uint16_t)(0x300 + ratio(d0, *d1)));          /* toward -y */
}

/* Mot_Atan2_LeftHalf - the dx < 0 half of Mot_Atan2: headings $100-$300.
 * In:  d0 = dx < 0, d1 = dy (low words)
 * Out: d2 low word = the heading; d0 negated (|dx|), d1 negated when dy < 0, and the divu result
 *      in one of them
 * Evidence: neg.w d0 / tst.w d1 / bmi / bne / move.w #$200,d2; then $200 - ratio, $100 + ratio,
 *           $200 + ratio or $300 - ratio; reached by bmi from Mot_Atan2
 */
void Mot_Atan2_LeftHalf(uint32_t *d0, uint32_t *d1, uint16_t *d2)
{
    set_low_word(d0, (uint16_t)-(int16_t)*d0);
    if ((int16_t)*d1 >= 0) {
        if ((uint16_t)*d1 == 0) {
            *d2 = 0x200;
            return;
        }
        if (!minus(*d0, *d1))
            *d2 = (uint16_t)(0x200 - ratio(d1, *d0));                  /* below the -x axis */
        else
            *d2 = (uint16_t)(0x100 + ratio(d0, *d1));                  /* toward +y */
        return;
    }
    set_low_word(d1, (uint16_t)-(int16_t)*d1);
    if (!minus(*d0, *d1))
        *d2 = (uint16_t)(0x200 + ratio(d1, *d0));                      /* above the -x axis */
    else
        *d2 = (uint16_t)(0x300 - ratio(d0, *d1));                      /* toward -y */
}
