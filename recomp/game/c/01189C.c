#include "game.h"

static constexpr uint16_t HUD_SLIDE_DISTANCE = 0x68;   /* px off home x */
static constexpr int16_t HUD_SLIDE_FRAMES_SUM = 0x1D1; /* 465 = 1 + 2 + ... + 30 */

/* divs.w: d / divisor, remainder in the upper word; an overflow leaves d as it was */
static uint32_t divs_w(uint32_t d, int16_t divisor)
{
    int32_t q = (int32_t)d / divisor, r = (int32_t)d % divisor;
    if (q < -32768 || q > 32767)
        return d;
    return ((uint32_t)(uint16_t)r << 16) | (uint16_t)q;
}

/* HUD_StartSlideIn - starts a panel sliding in: puts it 104 px off its screen edge, moving with
 * 31 braking steps of speed, so that HUD_StepPanelSlide's braking stops it on home x.
 * In:  rec = the player record (a6): +$A home x, +$25 bit 2 right-hand panel
 * Out: +$16 step = (home - start) / 4 / 465 x 4, +$12 speed = 31 steps, +$E = start - one step,
 *      +$25 bit 6 cleared; d0 = the speed, d4 = the step, d5 = +$E
 * Evidence: moveq #0,d4 / move.w $A(a6),d4 / move.l d4,d5 / move.w #$68,d0 / btst #2 / neg.w /
 *           add.w d0,d5 / swap d4 / swap d5 / sub.l d5,d4 / asr.l #2 / divs.w #$1D1 / muls.w #$1F
 *           / ext.l / asl.l #2 (both) / move.l $12, $16 / sub.l d4,d5 / move.l d5,$E / bclr #6;
 *           only caller HudOp_SlideIn
 */
void HUD_StartSlideIn(uint32_t *d0, uint32_t *d4, uint32_t *d5, PlayerRec *rec)
{
    uint32_t home = (uint16_t)rec->hud_x;
    uint16_t offset = HUD_SLIDE_DISTANCE;
    if (!(rec->flags & PLAYER_HUD_RIGHT))
        offset = (uint16_t)-offset;
    uint32_t start = (uint16_t)(home + offset);
    uint32_t step = (home << 16) - (start << 16);
    step = divs_w((uint32_t)((int32_t)step >> 2), HUD_SLIDE_FRAMES_SUM);
    int32_t speed = (int16_t)step * 31;
    *d4 = (uint32_t)(int32_t)(int16_t)step << 2;
    *d0 = (uint32_t)speed << 2;
    rec->panel_speed = (int32_t)*d0;
    rec->panel_accel = (int32_t)*d4;
    *d5 = (start << 16) - *d4;
    rec->panel_x = (int32_t)*d5;
    rec->flags = (uint8_t)(rec->flags & ~PLAYER_SLIDING_OUT);
}
