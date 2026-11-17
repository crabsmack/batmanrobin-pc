#include "game.h"

/* how far past its home x a panel slides out (and starts sliding in from) */
static constexpr int HUD_SLIDE_DISTANCE = 0x68;

/* HUD_StepPanelSlide - moves one player's HUD panel a frame along its slide. Sliding out (+$25
 * bit 6) it speeds up until it is 104 px past home x (left for player 1's panel, right for the
 * right-hand one), then stops there; sliding in it brakes by the step every frame until the
 * speed would change sign, then rests.
 * In:  rec = the player record (a6): +$A home x, +$E x and +$12 speed and +$16 step (16.16)
 * Out: +$E/+$12 updated; sliding out, bit 6 and the speed cleared once past home -/+ $68
 * Evidence: btst #6,$25(a6) / move.l $12,d0 / add.l $16,d0 / move.l $E,d1 / add.l d0,d1 / swap /
 *           sub.w $A / btst #2 / addi.w #$68 bpl or subi.w #$68 bmi / bclr #6 / moveq #0,d0; the
 *           braking side sub.l $16 with bpl/bmi by the speed's sign; called by HUD_Update for both
 *           records every frame
 */
void HUD_StepPanelSlide(PlayerRec *rec)
{
    if (rec->flags & PLAYER_SLIDING_OUT) {
        uint32_t speed = (uint32_t)rec->panel_speed + (uint32_t)rec->panel_accel;
        uint32_t x = (uint32_t)rec->panel_x + speed;
        int16_t past = (int16_t)((x >> 16) - (uint16_t)rec->hud_x);
        bool moving;
        if (!(rec->flags & PLAYER_HUD_RIGHT))
            moving = (int16_t)(past + HUD_SLIDE_DISTANCE) >= 0;
        else
            moving = (int16_t)(past - HUD_SLIDE_DISTANCE) < 0;
        if (!moving) {
            rec->flags = (uint8_t)(rec->flags & ~PLAYER_SLIDING_OUT);
            speed = 0;
        }
        rec->panel_speed = (int32_t)speed;
        rec->panel_x = (int32_t)x;
        return;
    }
    uint32_t speed = (uint32_t)rec->panel_speed;
    if (speed == 0)
        return;
    bool was_negative = (int32_t)speed < 0;
    speed -= (uint32_t)rec->panel_accel;
    if (((int32_t)speed < 0) != was_negative)   /* bpl / bmi: braked through 0 */
        speed = 0;
    rec->panel_speed = (int32_t)speed;
    rec->panel_x = (int32_t)((uint32_t)rec->panel_x + speed);
}
