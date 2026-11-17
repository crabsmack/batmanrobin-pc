#include "game.h"

enum {
    FLASH_COLOUR = 0x31,   /* the colour the flash greys: g_PalSource line 3 colour 1 ($E134) */
};

/* Attract_RooftopLightning - native of the rooftop object: the line tint steps (Tint_Step) on
 * palette line 3 and its negative on lines 0-2, and while sprite variable 0 counts down (set to
 * $20 at a flash), line 3's colour 1 = a grey from it ((v >> 1) & $E in all three channels), the
 * flash fading.
 * In:  spr = the object's sprite block (var[0]: the flash countdown); ccr's X
 * Out: g_Pal0-3Tint; while var[0] > 0: var[0] - 1, g_PalSource line 3 colour 1, g_Pal3Reload;
 *      d0.w = the grey (or var[0] - 1 < 0), d1 = Tint_Step's with the grey's channel in its low
 *      word; ccr = the last move.w's (or the subq's when the countdown ran out)
 * Evidence: Tint_Step / 3 x move.b d0 into g_Pal3Tint / neg.b / 9 x into g_Pal0-2Tint / move.w
 *           $22(a5),d0 / subq.w #1 / bmi / asr.w #1 / andi.w #$E / lsl.w #4 / or.w x2 / move.w
 *           d0,g_PalSource+$62 / st.b g_Pal3Reload; only caller Op2_CallNative at $048D02 in
 *           Scr2_048CE2; idle 926-1160 + 3925n
 */
void Attract_RooftopLightning(uint16_t *d0, uint32_t *d1, Vm2Sprite *spr, dk::Flags *ccr)
{
    uint8_t t = (uint8_t)*d0;
    Tint_Step(&t, d1);
    g_Pal3TintR = t;
    g_Pal3TintG = t;
    g_Pal3TintB = t;
    uint8_t dark = (uint8_t)-t;
    g_Pal0TintR = dark;
    g_Pal0TintG = dark;
    g_Pal0TintB = dark;
    g_Pal1TintR = dark;
    g_Pal1TintG = dark;
    g_Pal1TintB = dark;
    g_Pal2TintR = dark;
    g_Pal2TintG = dark;
    g_Pal2TintB = dark;

    uint16_t old = (uint16_t)spr->var[0];
    uint16_t count = (uint16_t)(old - 1);
    bool borrow = old == 0;   /* subq's X, kept by everything after it */
    if ((int16_t)count < 0) {
        *d0 = count;
        *ccr = dk::Flags::compare(old, 1, 2, borrow);
        return;
    }
    spr->var[0] = (int16_t)count;
    uint16_t level = (uint16_t)(((int16_t)count >> 1) & 0xE);
    *d1 = (*d1 & 0xFFFF0000u) | level;
    uint16_t grey = (uint16_t)((((level << 4) | level) << 4) | level);
    *d0 = grey;
    ((u16 *)g_PalSource)[FLASH_COLOUR] = grey;
    g_Pal3Reload = 0xFF;
    *ccr = dk::Flags::test(grey, 2, borrow);
}
