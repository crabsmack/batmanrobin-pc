#include "game.h"

/* Boss_Hatter_SwitchCruiseDir - switch for the hat's animation script: which way the hat is
 * cruising: 1/2 when the target x speed is negative/positive, else 3/4 for the target y speed
 * negative/positive, 0 when both are 0
 * In:  mot = the boss's motion block (a3: target_x +$24, target_y +$2C, integer parts)
 * Out: d0 = 1-4 (moveq); both 0: d0's low word = 0, its upper word kept (move.w)
 * Evidence: move.w $24(a3),d0 / bne / move.w $2C(a3),d0 / beq rts / bpl 4 / 3; bpl 2 / 1;
 *           Op2_CallNativeRelSwitch $03D77E five times in the hat's animation script Scr2_03D79E;
 *           TAS f113356-f115351
 */
void Boss_Hatter_SwitchCruiseDir(uint32_t *d0, const Vm2Motion *mot)
{
    int16_t speed_x = (int16_t)((uint32_t)mot->target_x >> 16);
    if (speed_x != 0) {
        *d0 = speed_x < 0 ? 1 : 2;
        return;
    }
    int16_t speed_y = (int16_t)((uint32_t)mot->target_y >> 16);
    if (speed_y == 0) {
        *d0 &= 0xFFFF0000u;
        return;
    }
    *d0 = speed_y < 0 ? 3 : 4;
}
