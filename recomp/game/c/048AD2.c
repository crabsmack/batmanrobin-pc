#include "game.h"

/* the skyscraper scene's a6 scripts (ROM): the plain-scene header and the graphics */
static constexpr uint32_t Scr6_0489E2 = 0x0489E2u;
static constexpr uint32_t Scr6_048A06 = 0x048A06u;

enum { START_Y = 0x6E8 };   /* plane A's layer starts this far down the tower */

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Attract_Skyscraper - third attract scene: the camera climbs a skyscraper under a red storm sky.
 * Plain scene, the graphics, plane A's map layer placed $6E8 px down and drawn whole, a V-int
 * object that flushes the layer's tile queue, the intro skip allowed from here on, the camera and
 * plane-B objects, the camera's view, then the Start-skippable scene loop.
 * In:  the registers the scene routines pass through
 * Out: ends through the scene loop (the camera's Scene_EndAfterDelay $C8, or Start);
 *      g_IntroNoSkip cleared
 * Evidence: lea Scr6_0489E2(pc),a6 / Scene_InitPlain / Scr6_048A06 VM6_Run / g_LayerA
 *           Layer_MoveYWithCamera $6E8 / Layer_DrawScreen / VIntObj_New(Layer_FlushTileQueueIfIdle)
 *           / sf.b g_IntroNoSkip / the two spawners / Camera_UpdateView / bra.w Scene_LoopSkippable;
 *           Tbl_00A152 entry 3; TAS f364; idle png f600 (a window close-up), f4800 (the top)
 */
void Attract_Skyscraper(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1,
                        uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0;   /* not inputs (measured): the callees set them */
    uint16_t w1 = (uint16_t)*d1, w6 = (uint16_t)*d6;
    Scene_InitPlain(&d0, &w1, &d2, &w6, &d7, a2, a3, a4, Scr6_0489E2);
    set_low_word(d1, w1);
    set_low_word(d6, w6);
    *a6 = Scr6_048A06;
    /* the script runs to its end (Op6_End) and leaves its registers to the rest */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);

    *a6 = dk::addr(g_LayerA);
    set_low_word(&d0, START_Y);
    Layer_MoveYWithCamera(START_Y, *a6);
    Layer_DrawScreen(&d2, d3, d4, d5, d6, d7, a3, a4, *a5, a6);
    VIntObj_New(&d0, at_Layer_FlushTileQueueIfIdle, a1, a6);
    g_IntroNoSkip = 0;

    Attract_SpawnSkyscraperCamera((uint16_t)*d4, (uint16_t)*d5, a3, a4, a6);
    Attract_SpawnPlaneBDriver((uint16_t)*d4, (uint16_t)*d5, &d7, a2, a3, a4, *a6);
    Camera_UpdateView(&d0, d1, &d2);
    dk::jump(at_Scene_LoopSkippable);
}
