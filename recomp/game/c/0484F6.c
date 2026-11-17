#include "game.h"

/* the asylum gate scene's a6 scripts (ROM): the plain-scene header and the graphics */
static constexpr uint32_t Scr6_047CD0 = 0x047CD0u;
static constexpr uint32_t Scr6_047D2A = 0x047D2Au;

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Story_AsylumGate - first scene of the idle story list: the Arkham Asylum gate under the moon.
 * Plain scene, the graphics, the camera object that pans up the gate, then the scene loop that
 * Start skips.
 * In:  the registers the scene routines pass through
 * Out: ends through the scene loop (the camera's Scene_EndAfterDelay $12C, or Start)
 * Evidence: lea Scr6_047CD0(pc),a6 / Scene_InitPlain / Scr6_047D2A VM6_Run / bsr.w
 *           Story_SpawnGateCamera / bra.w Scene_LoopSkippable; Tbl_00A136 entry 1; idle 1722 +
 *           3925n; png scenario-ending f140400 (the ARKHAM ASYLUM arch and the moon)
 */
void Story_AsylumGate(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2,
                      uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0;   /* not inputs (measured): the callees set them */
    uint16_t w1 = (uint16_t)*d1, w6 = (uint16_t)*d6;
    Scene_InitPlain(&d0, &w1, &d2, &w6, &d7, a2, a3, a4, Scr6_047CD0);
    set_low_word(d1, w1);
    set_low_word(d6, w6);
    *a6 = Scr6_047D2A;
    /* the script runs to its end (Op6_End) and leaves its registers to the spawner */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);
    w1 = (uint16_t)*d1;
    Story_SpawnGateCamera(&d0, &w1, (uint16_t)*d4, (uint16_t)*d5, &d7, a2, a3, a4, *a6);
    set_low_word(d1, w1);
    dk::jump(at_Scene_LoopSkippable);
}
