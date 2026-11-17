#include "game.h"

/* the player-select scene's a6 scripts (ROM): the plain-scene header (128x32 planes) and the
   graphics (two LZ blocks with their plane maps, the palettes) */
static constexpr uint32_t Scr6_049A56 = 0x049A56u;
static constexpr uint32_t Scr6_049A68 = 0x049A68u;

enum { SCROLL_LINES_A = 0x30 };   /* g_ScrollMode: plane A line scroll (g_HScrollLinesA) */

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* PlayerSelect_Run - scene routine of the player-select screen: plain scene, the graphics, Batman
 * chosen first, the chooser and the PLAYER / SELECT captions, plane A line scroll, then the scene
 * loop without the Start skip (the chooser ends the scene).
 * In:  the registers the scene routines pass through
 * Out: g_P1IsRobin = the choice; returns when the chooser's script ends the scene
 * Evidence: lea Scr6_049A56(pc),a6 / Scene_InitPlain / Scr6_049A68 VM6_Run / sf.b g_P1IsRobin /
 *           PlayerSelect_SpawnChooser / PlayerSelect_SpawnCaption / move.w #$30,g_ScrollMode /
 *           bra.w Scene_LoopUnskippableKeepMode; only caller Main_NewGame; TAS f412
 */
void PlayerSelect_Run(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a2, uint32_t *a3,
                      uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0, a1 = 0;   /* not inputs (measured): the callees set them */
    uint16_t w1 = (uint16_t)*d1, w6 = (uint16_t)*d6;
    Scene_InitPlain(&d0, &w1, &d2, &w6, &d7, a2, a3, a4, Scr6_049A56);
    set_low_word(d1, w1);
    set_low_word(d6, w6);
    *a6 = Scr6_049A68;
    /* the script runs to its end (Op6_End) and leaves its registers to the spawners */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, &a1, a2, a3, a4, a5, a6);
    g_P1IsRobin = 0;
    w1 = (uint16_t)*d1;
    PlayerSelect_SpawnChooser(&w1, (uint16_t)*d4, (uint16_t)*d5, a3, a4, a6);
    PlayerSelect_SpawnCaption(&d0, &w1, (uint16_t)*d4, (uint16_t)*d5, &d7, a2, a3, a4, *a6);
    set_low_word(d1, w1);
    g_ScrollMode = SCROLL_LINES_A;
    dk::jump(at_Scene_LoopUnskippableKeepMode);
}
