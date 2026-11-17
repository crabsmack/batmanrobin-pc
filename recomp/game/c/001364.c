#include "game.h"

/* The aim-at-a-point ops ($001364-$0013C9): each picks a target point, stores it with
 * Mot_SetTarget (which leaves dx/dy in d0/d1) and points the target heading at it (Mot_Atan2). */

/* the low word of a data register, the upper word kept (a .w move) */
static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* the shared tail: Mot_SetTarget at (x, y) in d0/d1's low words, then the heading from atan2 of
   the dx/dy it leaves, and NEXT */
static void aim(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, const u16 **script, Vm2Motion *mot,
                const Vm2Obj *self)
{
    uint16_t x = (uint16_t)*d0, y = (uint16_t)*d1;
    Mot_SetTarget(&x, &y, d3, mot, self);
    set_low_word(d0, x);
    set_low_word(d1, y);
    Mot_Atan2(d0, d1, d2);
    set_low_word(d2, (uint16_t)(*d2 & 0x3FF));
    mot->heading_target = (uint16_t)*d2;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op2_AimAtArgPoint - a2 op: (g_VM2Arg0, g_VM2Arg1) becomes the target point and the target
 * heading points at it.
 * In:  g_VM2Arg0 / g_VM2Arg1 (the words at their addresses: the longs' high halves)
 * Out: mot->point, flags, heading_target; d0/d1 = dx/dy then as Mot_Atan2 leaves them, d2 = the
 *      heading, d3 = |dy|; NEXT
 * Evidence: move.w (g_VM2Arg0).w,d0 / move.w (g_VM2Arg1).w,d1 / bsr.w Mot_SetTarget /
 *           jsr Mot_Atan2 / andi.w #$3FF,d2 / move.w d2,$A(a3); 56 TAS frames
 */
void Op2_AimAtArgPoint(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, const u16 **script, Vm2Motion *mot,
                       const Vm2Obj *self)
{
    set_low_word(d0, (uint16_t)(g_VM2Arg0 >> 16));
    set_low_word(d1, (uint16_t)(g_VM2Arg1 >> 16));
    aim(d0, d1, d2, d3, script, mot, self);
}

/* Op2_AimAtTargetOffset - a2 op: moves the target point by (dx, dy) and points the target
 * heading at it.
 * In:  script = words dx, dy; mot->point
 * Out: as Op2_AimAtArgPoint
 * Evidence: move.w (a2)+,d0 / move.w (a2)+,d1 / add.w $14(a3),d0 / add.w $16(a3),d1 / the same tail
 */
void Op2_AimAtTargetOffset(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, const u16 **script,
                           Vm2Motion *mot, const Vm2Obj *self)
{
    uint16_t dx = *(*script)++, dy = *(*script)++;
    set_low_word(d0, (uint16_t)(dx + mot->point_x));
    set_low_word(d1, (uint16_t)(dy + mot->point_y));
    aim(d0, d1, d2, d3, script, mot, self);
}

/* Op2_AimAtTargetArgOffset - a2 op: moves the target point by (g_VM2Arg0, g_VM2Arg1) and points
 * the target heading at it.
 * In:  g_VM2Arg0 / g_VM2Arg1 high words; mot->point
 * Out: as Op2_AimAtArgPoint
 * Evidence: move.w (g_VM2Arg0).w,d0 / move.w (g_VM2Arg1).w,d1 / add.w $14(a3),d0 / add.w $16(a3),d1
 */
void Op2_AimAtTargetArgOffset(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, const u16 **script,
                              Vm2Motion *mot, const Vm2Obj *self)
{
    set_low_word(d0, (uint16_t)((g_VM2Arg0 >> 16) + mot->point_x));
    set_low_word(d1, (uint16_t)((g_VM2Arg1 >> 16) + mot->point_y));
    aim(d0, d1, d2, d3, script, mot, self);
}
