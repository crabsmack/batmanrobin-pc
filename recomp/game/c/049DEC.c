#include "game.h"

/* the Clockwork Tortoise scene's plain-scene header, a6 graphics script and palette line 3 (ROM) */
static constexpr uint32_t Tbl_049CEE = 0x049CEEu;
static constexpr uint32_t Scr6_049D68 = 0x049D68u;
static constexpr uint32_t Tbl_049ED2 = 0x049ED2u;

enum { SHINE_FIRST = 8, SHINE_LAST = 15 };   /* the colours of palette line 0 the shine runs over */

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* SegaLogo_RotatePalette - rotates colours 8-15 of palette line 0 by one place (15 moves to 8)
 * and flags the line for reload: the running shine on the SEGA logo.
 * In:  none
 * Out: g_PalSource line 0 colours 8-15 rotated; g_Pal0Reload set; d0.w = the old colour 15,
 *      d7 = $FFFF (moveq #6 counted out by dbra)
 * Evidence: st.b g_Pal0Reload / lea g_PalSource+$1E,a0 / move.w (a0),d0 / 7 x move.w -(a0),2(a0) /
 *           move.w d0,(a0); called every 3rd frame from Scr2_049DCE; TAS f18-f155
 */
void SegaLogo_RotatePalette(uint16_t *d0, uint32_t *d7)
{
    g_Pal0Reload = 0xFF;
    u16 *line0 = (u16 *)g_PalSource;
    *d0 = line0[SHINE_LAST];
    for (int i = SHINE_LAST; i > SHINE_FIRST; i--)
        line0[i] = line0[i - 1];
    line0[SHINE_FIRST] = *d0;
    *d7 = 0x0000FFFFu;
}

/* ClockworkLogo_Run - scene routine of the Clockwork Tortoise (developer) logo, the last scene of
 * the idle story list: plain scene, the graphics, palette line 3, the logo object, then the scene
 * loop that Start may skip after 30 frames.
 * In:  the registers the scene routines pass through
 * Out: ends through the scene loop (after 230 frames, or on Start)
 * Evidence: lea Tbl_049CEE(pc),a6 / Scene_InitPlain / Scr6_049D68 VM6_Run / lea Tbl_049ED2,a0 /
 *           Pal_LoadLine3 / bsr.w ClockworkLogo_Spawn / bra.w Scene_LoopSkippableDelayed; last
 *           entry of Tbl_00A136; idle f3678; png w31-idle f3720 (the wind-up tortoise)
 */
void ClockworkLogo_Run(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1,
                       uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0;   /* not inputs (measured): the callees set them */
    uint16_t w1 = (uint16_t)*d1, w6 = (uint16_t)*d6;
    Scene_InitPlain(&d0, &w1, &d2, &w6, &d7, a2, a3, a4, Tbl_049CEE);
    set_low_word(d1, w1);
    set_low_word(d6, w6);
    *a6 = Scr6_049D68;
    /* the script runs to its end (Op6_End) and leaves its registers to the rest */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);
    uint32_t a0 = Tbl_049ED2;
    dk::Flags cc{};   /* the condition codes nothing here reads */
    Pal_LoadLine3(&a0, a1, &cc);
    w1 = (uint16_t)*d1;
    ClockworkLogo_Spawn(&d0, &w1, (uint16_t)*d4, (uint16_t)*d5, &d7, a2, a3, a4, *a6);
    set_low_word(d1, w1);
    dk::jump(at_Scene_LoopSkippableDelayed);
}
