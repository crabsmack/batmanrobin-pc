#include "game.h"

/* the epilogue scene's plain-scene header and a6 graphics script (ROM; labelled Scr2_ in the
   disassembly, but Scene_InitPlain and VM6_Run read them) */
static constexpr uint32_t Scr2_04A52A = 0x04A52Au;
static constexpr uint32_t Scr2_04A54E = 0x04A54Eu;

enum { FONT_TILE_BASE = 0xE42E };   /* the text font's tile base word */

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Ending_Epilogue - last ending scene: Batman's face close-up with the typewriter epilogue
 * ('REVENGE IS A DISH BEST SERVED CHILLED.' ...): plain scene, the graphics, the text-map font,
 * the narrator object, then the scene loop without the Start skip.
 * In:  the registers the scene routines pass through
 * Out: ends through the scene loop (the narrator's Scene_EndAfterDelay)
 * Evidence: lea Scr2_04A52A(pc),a6 / Scene_InitPlain / Scr2_04A54E VM6_Run / move.w #$E42E,d0 /
 *           TextMap_LoadFont / bsr.w Ending_SpawnEpilogueNarrator / bra.w Scene_LoopUnskippable;
 *           third call of Main_Ending; scenario-level-skip f19900
 */
void Ending_Epilogue(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2,
                     uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0, a0 = 0;   /* not inputs (measured): the callees set them */
    uint16_t w1 = (uint16_t)*d1, w6 = (uint16_t)*d6;
    Scene_InitPlain(&d0, &w1, &d2, &w6, &d7, a2, a3, a4, Scr2_04A52A);
    set_low_word(d1, w1);
    set_low_word(d6, w6);
    *a6 = Scr2_04A54E;
    /* the script runs to its end (Op6_End) and leaves its registers to the rest */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);
    set_low_word(&d0, FONT_TILE_BASE);
    TextMap_LoadFont(FONT_TILE_BASE, &a0);
    w1 = (uint16_t)*d1;
    Ending_SpawnEpilogueNarrator(&d0, &w1, (uint16_t)*d4, (uint16_t)*d5, &d7, a2, a3, a4, *a6);
    set_low_word(d1, w1);
    dk::jump(at_Scene_LoopUnskippable);
}
