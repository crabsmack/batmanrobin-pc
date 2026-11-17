#include "game.h"

/* the Batmobile scene's plain-scene header and a6 graphics script (ROM) */
static constexpr uint32_t Tbl_04A050 = 0x04A050u;
static constexpr uint32_t Scr6_04A0AA = 0x04A0AAu;

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* BatmobileIntro_Run - scene routine of the Batmobile drive shown before every stage (the first
 * scene of each stage's intro list): plain scene, the graphics, the animator object, then the
 * Start-skippable scene loop.
 * In:  the registers the scene routines pass through
 * Out: ends through the scene loop (after 120 frames, or on Start)
 * Evidence: lea Tbl_04A050(pc),a6 / Scene_InitPlain / Scr6_04A0AA VM6_Run / bsr.w
 *           BatmobileIntro_SpawnAnimator / bra.w Scene_LoopSkippable; first entry of Tbl_00A166 /
 *           Tbl_00A172 / Tbl_00A17E / Tbl_00A18A; TAS f477; png x-robin f720 (the Batmobile)
 */
void BatmobileIntro_Run(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1,
                        uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0;   /* not inputs (measured): the callees set them */
    uint16_t w1 = (uint16_t)*d1, w6 = (uint16_t)*d6;
    Scene_InitPlain(&d0, &w1, &d2, &w6, &d7, a2, a3, a4, Tbl_04A050);
    set_low_word(d1, w1);
    set_low_word(d6, w6);
    *a6 = Scr6_04A0AA;
    /* the script runs to its end (Op6_End) and leaves its registers to the spawner */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);
    w1 = (uint16_t)*d1;
    BatmobileIntro_SpawnAnimator(&d0, &w1, (uint16_t)*d4, (uint16_t)*d5, &d7, a2, a3, a4, *a6);
    set_low_word(d1, w1);
    dk::jump(at_Scene_LoopSkippable);
}
