#include "game.h"

/* the title scene's plain-scene header and a6 graphics script (ROM) */
static constexpr uint32_t Tbl_049674 = 0x049674u;
static constexpr uint32_t Tbl_049686 = 0x049686u;

enum { FONT_TILE_BASE = 0xE3E4 };   /* the menu font's tile base word */

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Title_Run - scene routine of the title screen: plain scene, the graphics (the BATMAN & ROBIN
 * logo), the text-map font, the menu director and the piecewise palette fade-in, then the scene
 * loop without the Start skip (the director handles Start).
 * In:  the registers the scene routines pass through
 * Out: ends through the scene loop: g_SceneResult 0 after 500 idle frames, 1 when START is chosen
 * Evidence: lea Tbl_049674(pc),a6 / Scene_InitPlain / Tbl_049686 VM6_Run / move.w #$E3E4,d0 /
 *           TextMap_LoadFont / bsr.w Title_SpawnMenu / bsr.w Title_StartPaletteFade / bra.w
 *           Scene_LoopUnskippable; only caller Main_Title; TAS f392; png x-options f420
 */
void Title_Run(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2,
               uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0, a0 = 0;   /* not inputs (measured): the callees set them */
    uint16_t w1 = (uint16_t)*d1, w6 = (uint16_t)*d6;
    Scene_InitPlain(&d0, &w1, &d2, &w6, &d7, a2, a3, a4, Tbl_049674);
    set_low_word(d1, w1);
    set_low_word(d6, w6);
    *a6 = Tbl_049686;
    /* the script runs to its end (Op6_End) and leaves its registers to the rest */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);
    set_low_word(&d0, FONT_TILE_BASE);
    TextMap_LoadFont(FONT_TILE_BASE, &a0);
    w1 = (uint16_t)*d1;
    Title_SpawnMenu(&w1, (uint16_t)*d4, (uint16_t)*d5, a2, a3, a4, a6);
    set_low_word(d1, w1);
    Title_StartPaletteFade(&d0, &d7, *a6);
    dk::jump(at_Scene_LoopUnskippable);
}
