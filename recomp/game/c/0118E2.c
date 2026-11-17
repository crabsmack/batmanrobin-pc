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

/* HUD_StartSlideOut - starts a panel sliding off its screen edge: the acceleration is the
 * distance to home x -/+ 104 px scaled as in HUD_StartSlideIn, the first speed 4 steps of it;
 * HUD_StepPanelSlide then speeds it up.
 * In:  rec = the player record (a6): +$A home x, +$E x (16.16), +$25 bit 2 right-hand panel
 * Out: +$16 = the step, +$12 = 4 steps, +$25 bit 6 set
 * Evidence: moveq #0,d1 / move.w $A(a6),d1 / move.w #$68,d0 / btst #2 / neg.w / add.w d0,d1 / swap
 *           / sub.l $E(a6),d1 / asr.l #2 / divs.w #$1D1 / ext.l / asl.l #2 / move.l d1,$16 / add.l
 *           d1,d1 (twice) / move.l d1,$12 / bset #6; only caller HudOp_SlideOut
 */
void HUD_StartSlideOut(PlayerRec *rec)
{
    uint16_t offset = HUD_SLIDE_DISTANCE;
    if (!(rec->flags & PLAYER_HUD_RIGHT))
        offset = (uint16_t)-offset;
    uint32_t target = (uint16_t)((uint16_t)rec->hud_x + offset);
    uint32_t d1 = (target << 16) - (uint32_t)rec->panel_x;
    d1 = divs_w((uint32_t)((int32_t)d1 >> 2), HUD_SLIDE_FRAMES_SUM);
    uint32_t step = (uint32_t)(int32_t)(int16_t)d1 << 2;
    rec->panel_accel = (int32_t)step;
    rec->panel_speed = (int32_t)(step << 2);
    rec->flags = (uint8_t)(rec->flags | PLAYER_SLIDING_OUT);
}
