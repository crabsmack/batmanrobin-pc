#include "game.h"

/* Spr_DrawDirect ($007D5E-$007D75). */

/* Spr_DrawDirect - draws one sprite frame straight into the SAT buffer, bypassing the display
 * list, clipped to the adjustable window (Spr_EmitClipped).
 * In:  entry = the frame-table entry (a0); x/y (d4/d5); attr (d6.w)
 * Out: SAT entries appended, g_SprCountLo and g_SATPtr stored back; d0, d2, d3 (moveq #0, then
 *      the last tile slot byte), d1, d6, d7, a0, a1, a2 as Spr_EmitClipped leaves them (d1 an output
 *      since the debug readouts: Debug_DrawHexBytes hands it on to Debug_FreeCamStep's caller)
 * Evidence: movea.w (g_SATPtr).w,a1 / move.w (g_SprCount).w,d2 / moveq #0,d3 /
 *           bsr.w Spr_EmitClipped / move.b d2,(g_SprCountLo).w / move.w a1,(g_SATPtr).w; callers
 *           $11A1C and HUD_DrawDigits ($15940, after Spr_ChangeFrame)
 */
void Spr_DrawDirect(uint32_t *d0, uint32_t *d1, uint16_t *d2, uint32_t *d3, uint16_t x, uint16_t y, uint16_t *attr,
                    uint16_t *d7, uint32_t *entry, uint32_t *sat, uint32_t *a2)
{
    *sat = (uint32_t)(int32_t)(int16_t)g_SATPtr;
    *d2 = g_SprCount;
    uint16_t slot = 0;
    Spr_EmitClipped(d0, d1, d2, &slot, x, y, attr, d7, entry, sat, a2);
    *d3 = slot;
    g_SprCountLo = (uint8_t)*d2;
#if DK_MODE_NATIVE
    /* phase 6 (E3): the GO! sign's entries tagged for the widescreen view's pinned HUD (ws_draw.h) */
    if (int kind = ws_hud_direct_kind()) {
        ws_hud_tag(g_SATPtr, (uint16_t)*sat, kind);
        ws_hud_direct_end();
    }
#endif
    g_SATPtr = (uint16_t)*sat;
}
