#include "game.h"

/* The heading ops ($0012E4-$001363): headings are 0-$3FF (1024 a turn, 0 = +x, $100 = +y); the
 * current one is 16.16 at a3 +$C, the target a word at +$A. */

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_SetCurHeadingArg0 - a2 op: the current heading's integer part = g_VM2Arg0's high word &
 * $3FF (its fraction kept).
 * In:  g_VM2Arg0 (the word at its address, the long's high half)
 * Out: heading = the new integer part; mot->heading; NEXT
 * Evidence: move.w (g_VM2Arg0).w,d0 / andi.w #$3FF,d0 / move.w d0,$C(a3); 3.8k TAS frames
 */
void Op2_SetCurHeadingArg0(uint16_t *heading, const u16 **script, Vm2Motion *mot)
{
    uint32_t handler;
    *heading = (uint16_t)((g_VM2Arg0 >> 16) & 0x3FF);
    mot->heading = (int32_t)((uint32_t)*heading << 16 | ((uint32_t)mot->heading & 0xFFFF));
    next(&handler, script);
}

/* Op2_AddHeading - a2 op: turns the target heading by the operand, wrapping at a full turn.
 * In:  script = word delta
 * Out: mot->heading_target; NEXT
 * Evidence: move.w (a2)+,d0 / add.w $A(a3),d0 / andi.w #$3FF,d0 / move.w d0,$A(a3); executed in the TAS
 */
void Op2_AddHeading(const u16 **script, Vm2Motion *mot)
{
    uint32_t handler;
    mot->heading_target = (uint16_t)((*(*script)++ + mot->heading_target) & 0x3FF);
    next(&handler, script);
}

/* Op2_SetHeadingNow - a2 op: both the target and the current heading (fraction 0) = operand &
 * $3FF, so it takes effect without turning.
 * In:  script = word heading
 * Out: mot->heading_target, mot->heading; NEXT
 * Evidence: moveq #0,d0 / move.w (a2)+,d0 / andi.w #$3FF / move.w d0,$A(a3) / swap / move.l d0,$C(a3)
 */
void Op2_SetHeadingNow(const u16 **script, Vm2Motion *mot)
{
    uint32_t handler;
    uint16_t h = (uint16_t)(*(*script)++ & 0x3FF);
    mot->heading_target = h;
    mot->heading = (int32_t)((uint32_t)h << 16);
    next(&handler, script);
}

/* Op2_HeadingFromVel - a2 op: points the target and current headings along the velocity (atan2
 * of the velocity >> 8, Mot_Atan2) and clears both target speeds.
 * In:  mot->vel_x / vel_y
 * Out: mot->heading_target, heading (fraction 0), target_x = target_y = 0; d0/d1 as Mot_Atan2
 *      leaves them, d2 = the heading << 16; NEXT
 * Evidence: move.l $18(a3),d0 / move.l $1C(a3),d1 / asr.l #8 / jsr Mot_Atan2 / andi.w #$3FF,d2 /
 *           swap / clr.w / move.l d2,$C(a3) / clr.l $24(a3) / clr.l $2C(a3); the tail of
 *           Op2_BranchPastTarget's branch; executed in the TAS
 */
void Op2_HeadingFromVel(uint32_t *d0, uint32_t *d1, uint32_t *d2, const u16 **script, Vm2Motion *mot)
{
    uint32_t handler;
    *d0 = (uint32_t)((int32_t)mot->vel_x >> 8);
    *d1 = (uint32_t)((int32_t)mot->vel_y >> 8);
    Mot_Atan2(d0, d1, d2);
    uint16_t h = (uint16_t)(*d2 & 0x3FF);
    mot->heading_target = h;
    *d2 = (uint32_t)h << 16;
    mot->heading = (int32_t)*d2;
    mot->target_x = 0;
    mot->target_y = 0;
    next(&handler, script);
}
