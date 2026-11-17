#include "game.h"

/* the SEGA logo scene's plain-scene header and a6 graphics script (ROM) */
static constexpr uint32_t Tbl_049CCA = 0x049CCAu;
static constexpr uint32_t Scr6_049D08 = 0x049D08u;

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* SegaLogo_Run - scene routine of the SEGA logo (the first attract scene, also after GAME OVER):
 * plain scene, the logo graphics, the palette-cycling object, then the scene loop that Start may
 * skip after 30 frames.
 * In:  the registers the scene routines pass through
 * Out: returns after 120 frames (the object's Scene_EndAfterDelay) or on Start
 * Evidence: lea Tbl_049CCA(pc),a6 / Scene_InitPlain / Scr6_049D08 VM6_Run / bsr.w
 *           SegaLogo_SpawnCycler / bra.w Scene_LoopSkippableDelayed; first entry of Tbl_00A152
 *           (Main_Attract); TAS f10; png x-attract f100
 */
void SegaLogo_Run(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2,
                  uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0;   /* not inputs (measured): the callees set them */
    uint16_t w1 = (uint16_t)*d1, w6 = (uint16_t)*d6;
    Scene_InitPlain(&d0, &w1, &d2, &w6, &d7, a2, a3, a4, Tbl_049CCA);
    set_low_word(d1, w1);
    set_low_word(d6, w6);
    *a6 = Scr6_049D08;
    /* the script runs to its end (Op6_End) and leaves its registers to the spawner */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);
    w1 = (uint16_t)*d1;
    SegaLogo_SpawnCycler(&d0, &w1, (uint16_t)*d4, (uint16_t)*d5, &d7, a2, a3, a4, *a6);
    set_low_word(d1, w1);
    dk::jump(at_Scene_LoopSkippableDelayed);
}
