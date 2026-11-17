#include "game.h"

/* the asylum facade scene's a6 scripts (ROM): the plain-scene header and the graphics */
static constexpr uint32_t Scr6_047D06 = 0x047D06u;
static constexpr uint32_t Scr6_047DF4 = 0x047DF4u;

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Story_TowerScroll - native of the tower scene's object: plane B's V-scroll = -y, so plane B
 * follows the object's drop.
 * In:  self = the object
 * Out: g_VScrollB; d0.w = it
 * Evidence: move.w $42(a6),d0 / neg.w d0 / move.w d0,g_VScrollB; only caller Op2_CallNativeRel at
 *           $048702 in Scr2_0486DE; scenario-ending 141608-145679
 */
uint16_t Story_TowerScroll(const StoryCamObj *self)
{
    uint16_t v = (uint16_t)-(uint16_t)self->y;
    g_VScrollB = v;
    return v;
}

/* Story_AsylumFacade - fifth story scene: the asylum's wall with barred windows and a figure on
 * it, lit by lightning. Plain scene, the graphics, the camera object, then the Start-skippable
 * scene loop.
 * In:  the registers the scene routines pass through
 * Out: ends through the scene loop (the camera's Scene_EndAfterDelay $12C, or Start)
 * Evidence: lea Scr6_047D06(pc),a6 / Scene_InitPlain / Scr6_047DF4 VM6_Run / bsr.w
 *           Story_SpawnFacadeCamera / bra.w Scene_LoopSkippable; Tbl_00A136 entry 5; idle png f11400
 */
void Story_AsylumFacade(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2,
                        uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0;   /* not inputs (measured): the callees set them */
    uint16_t w1 = (uint16_t)*d1, w6 = (uint16_t)*d6;
    Scene_InitPlain(&d0, &w1, &d2, &w6, &d7, a2, a3, a4, Scr6_047D06);
    set_low_word(d1, w1);
    set_low_word(d6, w6);
    *a6 = Scr6_047DF4;
    /* the script runs to its end (Op6_End) and leaves its registers to the spawner */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);
    w1 = (uint16_t)*d1;
    Story_SpawnFacadeCamera(&d0, &w1, (uint16_t)*d4, (uint16_t)*d5, &d7, a2, a3, a4, *a6);
    set_low_word(d1, w1);
    dk::jump(at_Scene_LoopSkippable);
}
