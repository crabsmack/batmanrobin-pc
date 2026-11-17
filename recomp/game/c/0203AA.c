#include "game.h"

static constexpr int16_t BALLOON_ARRIVE_RANGE = 4;   /* within this many px of its height it turns back */

/* Stage1_BalloonBob - balloon-bomb native: cruises the balloon vertically toward the screen height
 * height[0] and, once within 4 px of it, swaps the two heights, so it bobs between them
 * In:  motion = its motion block (a3: cruise_y), self = the balloon (a6: y, height[0..1])
 * Out: motion->target_y = +cruise_y (above the height: going down) or -cruise_y (at or below it);
 *      the heights swapped when it arrived; d0 = the target speed, d1/d2 = the two heights as
 *      stored (whole registers: exg.l), d3 = the screen-y distance moved by 4 toward 0
 * Evidence: move.l $34(a3),d0 / move.w $2A/$2C/$42(a6),d1/d2/d3 / sub.w g_CameraY,d3 / sub.w d1,d3 /
 *           blt: addq.w #4 / bmi / else neg.l d0 / subq.w #4 / bpl / exg.l d1,d2 / move.l d0,$2C(a3)
 *           / move.w d1/d2 to $2A/$2C(a6); every frame of the Scr2_0203E0 objects (TAS f13768-f18211,
 *           3270 frames); RAM f14400 four balloons at screen y 100-200 bobbing
 */
void Stage1_BalloonBob(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, Vm2Motion *motion,
                       BalloonBombObj *self)
{
    *d0 = (uint32_t)motion->cruise_y;
    *d1 = (*d1 & 0xFFFF0000u) | (uint16_t)self->height[0];
    *d2 = (*d2 & 0xFFFF0000u) | (uint16_t)self->height[1];
    int16_t screen_y = (int16_t)(self->y - (uint16_t)(g_CameraY >> 16));   /* the camera's integer part */
    bool arrived;
    if (screen_y < (int16_t)*d1) {    /* sub.w d1,d3 / blt: above the height (signed compare) */
        *d3 = (uint16_t)(screen_y - (int16_t)*d1 + BALLOON_ARRIVE_RANGE);
        arrived = (int16_t)*d3 >= 0;  /* addq.w #4 / bmi: still more than 4 px away */
    } else {
        *d0 = (uint32_t)-(int32_t)*d0;   /* neg.l: up */
        *d3 = (uint16_t)(screen_y - (int16_t)*d1 - BALLOON_ARRIVE_RANGE);
        arrived = (int16_t)*d3 < 0;   /* subq.w #4 / bpl: still more than 4 px away */
    }
    if (arrived) {                    /* exg.l d1,d2 */
        uint32_t t = *d1;
        *d1 = *d2;
        *d2 = t;
    }
    motion->target_y = (int32_t)*d0;
    self->height[0] = (int16_t)*d1;
    self->height[1] = (int16_t)*d2;
}
