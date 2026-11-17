#include "game.h"

enum { GREEN_FLICKER = 0xFE };   /* g_Pal1TintG toggles between 0 and -2 */

/* Continue_ScrollPicture - per-frame native of the continue screen: plane A (Mr. Freeze's head) at
 * the object's bobbing x / y plus the shake, plane B at half the shake; and while the fade is not
 * above normal, palette line 1's green tint flickers 0 / -2 every frame (the countdown monitors),
 * otherwise it is cleared.
 * In:  self = the controller (x, y: the oscillator)
 * Out: g_HScrollA, g_VScrollA, g_VScrollB, g_Pal1TintG; d0.w = plane A's V-scroll, d1.w = half
 *      the shake
 * Evidence: move.w $3E(a6),g_HScrollA / g_ShakeY + $42(a6) into g_VScrollA / asr.w #1 into
 *           g_VScrollB / tst.b g_FadeLevelVInt / ble / eori.b #$FE,g_Pal1TintG or clr.b;
 *           Op2_CallNativeRel from $491E8; png f42600; scenario-continue f41999-f42251
 */
void Continue_ScrollPicture(uint16_t *d0, uint16_t *d1, const Vm2Obj *self)
{
    g_HScrollA = (uint16_t)self->x;
    uint16_t shake = (uint16_t)((uint32_t)g_ShakeY >> 16);   /* the 16.16 shake's integer part */
    *d0 = (uint16_t)(self->y + shake);
    g_VScrollA = *d0;
    *d1 = (uint16_t)((int16_t)shake >> 1);
    g_VScrollB = *d1;
    if ((int8_t)g_FadeLevelVInt > 0)
        g_Pal1TintG = 0;
    else
        g_Pal1TintG = (uint8_t)(g_Pal1TintG ^ GREEN_FLICKER);
}
