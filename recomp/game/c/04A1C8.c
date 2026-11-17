#include "game.h"

enum {
    STREAK_GROUPS = 4,         /* palette line 0 colours 2-13: four groups of three */
    FIRST_STREAK = 2,
    BUMP_X = -0x140,           /* plane A 320 px to the right ($FEC0) */
};

/* BatmobileIntro_Animate - per-frame native of the stage-opening object (the Batmobile racing
 * through the night): every 4th frame rotates each of four 3-colour groups of palette line 0
 * (colours 2-13: the speed streaks) by one place, and on 2 of every 6 frames shows plane A 320 px
 * to the right and 1 px lower (the car's bump).
 * In:  self = the opening object (its two timers)
 * Out: g_HScrollA = 0 or -$140, g_VScrollA = 0 or 1, g_Pal0Reload set after a rotation; scroll_x /
 *      scroll_y = those values (moveq: whole registers); a rotation: d2 = the last group's third
 *      colour (movem.w: sign-extended), d7 = $FFFF
 * Evidence: subq.w #1,$2A(a6) bcc / move.w #3 / st.b g_Pal0Reload / lea $FFFFE0D6 (g_PalSource+4) /
 *           movem.w (a0),d0-d2 / d2,d0,d1 back x4; moveq #0,d0/d1 / subq.w #1,$2C(a6) bcc / move.w
 *           #5 / btst #2,$2D(a6) / move.w #$FEC0,d0 / moveq #1,d1; 140 frames per stage start
 */
void BatmobileIntro_Animate(uint32_t *scroll_x, uint32_t *scroll_y, uint32_t *d2, uint32_t *d7, BatmobileObj *self)
{
    uint16_t t = self->pal_timer;
    self->pal_timer = (uint16_t)(t - 1);
    if (t == 0) {   /* subq / bcc: wrapped below 0 */
        self->pal_timer = 3;
        g_Pal0Reload = 0xFF;
        u16 *c = (u16 *)g_PalSource + FIRST_STREAK;
        for (int g = 0; g < STREAK_GROUPS; g++, c += 3) {
            int16_t a = (int16_t)c[0], b = (int16_t)c[1], z = (int16_t)c[2];
            c[0] = (uint16_t)z;
            c[1] = (uint16_t)a;
            c[2] = (uint16_t)b;
            *d2 = (uint32_t)(int32_t)z;   /* movem.w loads d0-d2 sign-extended */
        }
        *d7 = 0x0000FFFFu;   /* moveq #3,d7 counted down by dbra */
    }

    *scroll_x = 0;
    *scroll_y = 0;
    uint16_t b = self->bump_timer;
    self->bump_timer = (uint16_t)(b - 1);
    if (b == 0)
        self->bump_timer = 5;
    if (self->bump_timer & 4) {
        *scroll_x = (uint16_t)BUMP_X;   /* move.w over the moveq #0: upper word 0 */
        *scroll_y = 1;
    }
    g_HScrollA = (uint16_t)*scroll_x;
    g_VScrollA = (uint16_t)*scroll_y;
}
