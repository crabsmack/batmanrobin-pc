#include "game.h"

/* the copyright scene's plain-scene header and a6 graphics script (ROM) */
static constexpr uint32_t Tbl_049CDC = 0x049CDCu;
static constexpr uint32_t Scr6_049D38 = 0x049D38u;

enum { FONT_TILE_BASE = 0xE4E8 };   /* the text font's tile base word (priority, palette 3, tile $4E8) */

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Copyright_Run - scene routine of the copyright notice (the second attract scene): plain scene,
 * the graphics (the bat emblem), the text-map font, the notice object, then the scene loop that
 * Start may skip after 30 frames.
 * In:  the registers the scene routines pass through
 * Out: ends through the scene loop (after 180 frames, or on Start)
 * Evidence: lea Tbl_049CDC(pc),a6 / Scene_InitPlain / Scr6_049D38 VM6_Run / move.w #$E4E8,d0 /
 *           TextMap_LoadFont / bsr.w Copyright_SpawnText / bra.w Scene_LoopSkippableDelayed;
 *           second entry of Tbl_00A152 (Main_Attract); TAS f158; png x-attract f200
 */
void Copyright_Run(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2,
                   uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0, a0 = 0;   /* not inputs (measured): the callees set them */
    uint16_t w1 = (uint16_t)*d1, w6 = (uint16_t)*d6;
    Scene_InitPlain(&d0, &w1, &d2, &w6, &d7, a2, a3, a4, Tbl_049CDC);
    set_low_word(d1, w1);
    set_low_word(d6, w6);
    *a6 = Scr6_049D38;
    /* the script runs to its end (Op6_End) and leaves its registers to the rest */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);
    set_low_word(&d0, FONT_TILE_BASE);
    TextMap_LoadFont(FONT_TILE_BASE, &a0);
    w1 = (uint16_t)*d1;
    Copyright_SpawnText(&d0, &w1, (uint16_t)*d4, (uint16_t)*d5, &d7, a2, a3, a4, *a6);
    set_low_word(d1, w1);
    dk::jump(at_Scene_LoopSkippableDelayed);
}
