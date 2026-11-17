#include "game.h"

/* Title_DimPicture - darkens the title picture behind the menu text: the red, green and blue tints
 * of palette lines 0-2 to -4 (Fade_Step adds them to the fade level).
 * In:  none
 * Out: g_Pal0-2Tint{R,G,B} = -4; returns -4 (the moveq that loaded it)
 * Evidence: moveq #-4,d0 then nine move.b d0 to $F4C7-$F4D1 (line 3 untouched); Op2_CallNativeRel from
 *           $4972A when the menu opens; png x-options f420: the logo dimmed under the OPTIONS text
 */
uint32_t Title_DimPicture(void)
{
    const int8_t dim = -4;
    g_Pal0TintR = (uint8_t)dim;
    g_Pal1TintR = (uint8_t)dim;
    g_Pal2TintR = (uint8_t)dim;
    g_Pal0TintG = (uint8_t)dim;
    g_Pal1TintG = (uint8_t)dim;
    g_Pal2TintG = (uint8_t)dim;
    g_Pal0TintB = (uint8_t)dim;
    g_Pal1TintB = (uint8_t)dim;
    g_Pal2TintB = (uint8_t)dim;
    return (uint32_t)(int32_t)dim;
}
