#include "game.h"

/* the freeze cell scene's plain-scene header and a6 graphics script (ROM; labelled Scr2_ in the
   disassembly, but Scene_InitPlain and VM6_Run read them) */
static constexpr uint32_t Scr2_04A53C = 0x04A53Cu;
static constexpr uint32_t Scr2_04A572 = 0x04A572u;

enum { FONT_TILE_BASE = 0xE3AE };   /* the text font's tile base word */

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Ending_FreezeCell - second ending scene: the story of Mr. Freeze in his Arkham cell (typewriter
 * text, his head at the barred window, the snow globe): plain scene, the graphics, the text-map
 * font, the narrator object, then the scene loop without the Start skip.
 * In:  the registers the scene routines pass through
 * Out: ends through the scene loop (the narrator's Scene_EndAfterDelay)
 * Evidence: lea Scr2_04A53C(pc),a6 / Scene_InitPlain / Scr2_04A572 VM6_Run / move.w #$E3AE,d0 /
 *           TextMap_LoadFont / bsr.w Ending_SpawnCellNarrator / bra.w Scene_LoopUnskippable;
 *           second call of Main_Ending; scenario-level-skip f15089
 */
void Ending_FreezeCell(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2,
                       uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0, a0 = 0;   /* not inputs (measured): the callees set them */
    uint16_t w1 = (uint16_t)*d1, w6 = (uint16_t)*d6;
    Scene_InitPlain(&d0, &w1, &d2, &w6, &d7, a2, a3, a4, Scr2_04A53C);
    set_low_word(d1, w1);
    set_low_word(d6, w6);
    *a6 = Scr2_04A572;
    /* the script runs to its end (Op6_End) and leaves its registers to the rest */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);
    set_low_word(&d0, FONT_TILE_BASE);
    TextMap_LoadFont(FONT_TILE_BASE, &a0);
    w1 = (uint16_t)*d1;
    Ending_SpawnCellNarrator(&d0, &w1, (uint16_t)*d4, (uint16_t)*d5, &d7, a2, a3, a4, *a6);
    set_low_word(d1, w1);
    dk::jump(at_Scene_LoopUnskippable);
}
