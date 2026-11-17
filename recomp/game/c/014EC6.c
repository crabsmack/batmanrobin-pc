#include "game.h"

/* Mot_Atan2's entry and its axis exits ($014EC6-$014EE3); the two halves with a dx are the next
 * file (Mot_Atan2_RightHalf / Mot_Atan2_LeftHalf). Headings: 0 = +x, $100 = +y (down), $200 = -x,
 * $300 = -y, 1024 a turn. */

enum { HEADING_DOWN = 0x100, HEADING_UP = 0x300 };

/* Mot_Atan2 - approximate atan2: the heading of the vector (dx, dy), linear in the ratio of the
 * minor to the major component inside each octant.
 * In:  d0 = dx, d1 = dy (low words; screen y down)
 * Out: d2 = the heading (0 a long, for (0, 0)); the dx halves change d0/d1 (see them)
 * Evidence: tst.w d0 / bmi Mot_Atan2_LeftHalf / bne Mot_Atan2_RightHalf / tst.w d1 / bmi / bne /
 *           moveq #0,d2; callers the aim ops via Mot_SetTarget ($1364/$1382/$13A4), $29534, $334D2,
 *           $37708; 15758 TAS frames
 */
void Mot_Atan2(uint32_t *d0, uint32_t *d1, uint32_t *d2)
{
    int16_t dx = (int16_t)*d0, dy = (int16_t)*d1;
    if (dx < 0) {
        dk::jump(at_Mot_Atan2_LeftHalf);
        return;
    }
    if (dx != 0) {
        dk::jump(at_Mot_Atan2_RightHalf);
        return;
    }
    if (dy < 0)
        *d2 = (*d2 & 0xFFFF0000u) | HEADING_UP;
    else if (dy != 0)
        *d2 = (*d2 & 0xFFFF0000u) | HEADING_DOWN;
    else
        *d2 = 0;
}

/* Mot_Atan2_Zero - the heading-0 exit of Mot_Atan2 ((0, 0), and dx > 0 with dy = 0).
 * In:  none
 * Out: d2 = 0 (moveq)
 * Evidence: moveq #0,d2 / rts; reached from Mot_Atan2 and by beq from Mot_Atan2_RightHalf
 */
uint32_t Mot_Atan2_Zero(void)
{
    return 0;
}

/* Mot_Atan2_Down - the dx = 0, dy > 0 exit of Mot_Atan2: heading $100.
 * In:  none
 * Out: d2 low word = $100
 * Evidence: move.w #$100,d2 / rts after tst.w d1 / bne
 */
uint16_t Mot_Atan2_Down(void)
{
    return HEADING_DOWN;
}

/* Mot_Atan2_Up - the dx = 0, dy < 0 exit of Mot_Atan2: heading $300.
 * In:  none
 * Out: d2 low word = $300
 * Evidence: move.w #$300,d2 / rts after tst.w d1 / bmi
 */
uint16_t Mot_Atan2_Up(void)
{
    return HEADING_UP;
}
