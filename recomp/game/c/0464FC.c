#include "game.h"

enum { FREEZE_DROP_MAX = 0x50 };   /* px below the platform where he lands */

/* Boss_Freeze_DropStep - one frame of Mr. Freeze's drop off his broken hover platform: adds the
 * frame's y move to the drop offset, clamps it at $50 px and draws him at the platform's x and y +
 * the offset
 * In:  d2 = this frame's y move (16.16, from Op2_StepVelocity); platform = the hover-platform block
 *      (a3 = $12(a6): +$C drop offset); self = the boss (a6)
 * Out: d0 = 0 still falling, 1 landed (moveq); d2 = the new offset swapped (the fraction in the high
 *      word), its low word replaced by the drawn y; self->draw_x / draw_y set
 * Evidence: add.l $C(a3),d2 / move.l d2,$C(a3) / swap d2 / moveq #0,d0 / cmpi.w #$50,d2 / blt / moveq
 *           #1,d0 / move.w #$50,d2 / add.w $42(a6),d2 / move.w d2,$50(a6) / move.w $3E(a6),$4E(a6);
 *           only caller Op2_CallNativeRelSwitch at $046434 in Scr2_046176; TAS 130401-130448
 */
void Boss_Freeze_DropStep(uint32_t *d0, uint32_t *d2, FreezePlatform *platform, S4Obj *self)
{
    uint32_t drop = *d2 + (uint32_t)(int32_t)platform->drop;
    platform->drop = (int32_t)drop;
    uint32_t swapped = (drop << 16) | (drop >> 16);
    int16_t below = (int16_t)(uint16_t)swapped;   /* the offset's integer part */
    *d0 = 0;
    if (below >= FREEZE_DROP_MAX) {
        *d0 = 1;
        below = FREEZE_DROP_MAX;   /* the stored offset keeps running on; only the drawn y stops */
    }
    uint16_t y = (uint16_t)(below + self->y);
    *d2 = (swapped & 0xFFFF0000u) | y;
    self->draw_y = (int16_t)y;
    self->draw_x = self->x;
}
