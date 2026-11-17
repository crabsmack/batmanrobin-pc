#include "game.h"

/* the rooftop scene's a6 scripts (ROM): the plain-scene header and the graphics */
static constexpr uint32_t Scr6_0489F4 = 0x0489F4u;
static constexpr uint32_t Scr6_048A86 = 0x048A86u;

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* the integer part of a 16.16 camera coordinate (`move.w (g_CameraY).w`) */
static uint16_t camera_y_px(void) { return (uint16_t)((uint32_t)g_CameraY >> 16); }

/* the lightning: palette line 3 brightened by the tint level, lines 0-2 darkened by as much */
static void set_line_tints(uint8_t t)
{
    g_Pal3TintR = t;
    g_Pal3TintG = t;
    g_Pal3TintB = t;
    uint8_t dark = (uint8_t)-t;
    g_Pal0TintR = dark;
    g_Pal0TintG = dark;
    g_Pal0TintB = dark;
    g_Pal1TintR = dark;
    g_Pal1TintG = dark;
    g_Pal1TintB = dark;
    g_Pal2TintR = dark;
    g_Pal2TintG = dark;
    g_Pal2TintB = dark;
}

/* Attract_SkyscraperScroll - native of the skyscraper camera: the camera's y = the object's,
 * plane A's map layer scrolled to it (Layer_ScrollStep, Scroll_SetPlaneA), plane B's V-scroll =
 * half the camera y and its H-scroll 0; then the line tint steps (Tint_Step): line 3 brighter,
 * lines 0-2 darker by the same amount (the lightning).
 * In:  self = the camera object; the layer routines' registers
 * Out: g_CameraY's integer part, g_LayerA, the scroll words, g_Pal0-3Tint; d0 = Scroll_SetPlaneA's
 *      x with the half camera y in its low word and -tint in its low byte; d1 = Tint_Step's;
 *      d2-d7 the layer routines'
 * Evidence: move.w $42(a6),d0 / move.w d0,g_CameraY / sub.w $2(a6),d0 / move.w d0,$6(a6) /
 *           Layer_ScrollStep / Scroll_SetPlaneA / asr.w #1 into g_VScrollB / clr.w g_HScrollB /
 *           Tint_Step, neg.b between line 3 and lines 0-2; only caller Op2_CallNativeRelSaved at
 *           $048B46 in Scr2_048B2A; idle 391-926 + 3925n
 */
void Attract_SkyscraperScroll(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint32_t *d5,
                              uint16_t *d6, uint16_t *d7, const Vm2Obj *self)
{
    uint16_t y = (uint16_t)self->y;
    g_CameraY = ((uint32_t)y << 16) | ((uint32_t)g_CameraY & 0xFFFFu);
    MapLayer *layer = (MapLayer *)g_LayerA;
    layer->pending_y = (int16_t)(uint16_t)(y - layer->y);
    uint32_t a0 = 0, a2 = 0, a3 = 0;   /* clobbers */
    Layer_ScrollStep(d1, d2, d3, d4, d5, d6, d7, &a0, &a2, &a3, layer);

    uint32_t plane = dk::addr(layer);
    uint16_t fine = (uint16_t)*d2;
    Scroll_SetPlaneA(d0, d1, &fine, &plane);
    set_low_word(d2, fine);

    set_low_word(d0, (uint16_t)((int16_t)camera_y_px() >> 1));
    g_VScrollB = (uint16_t)*d0;
    g_HScrollB = 0;
    uint8_t t = (uint8_t)*d0;
    Tint_Step(&t, d1);
    set_line_tints(t);
    *d0 = (*d0 & 0xFFFFFF00u) | (uint8_t)-t;
}

/* Attract_PlaneBFollowObj - native of the skyscraper's second object: plane B's V-scroll = the
 * camera y - its y and its H-scroll = its x, so plane B's picture moves with the object.
 * In:  self = the object
 * Out: g_VScrollB, g_HScrollB; d0.w = the V-scroll
 * Evidence: move.w g_CameraY,d0 / sub.w $42(a6),d0 / move.w d0,g_VScrollB / move.w
 *           $3E(a6),g_HScrollB; only caller Op2_CallNativeRelSaved at $048BCE in Scr2_048B9A
 */
uint16_t Attract_PlaneBFollowObj(const Vm2Obj *self)
{
    uint16_t v = (uint16_t)(camera_y_px() - (uint16_t)self->y);
    g_VScrollB = v;
    g_HScrollB = (uint16_t)self->x;
    return v;
}

/* Attract_Rooftop - fourth attract scene: Batman and Robin standing on a rooftop skylight under
 * the red sky. Plain scene, the graphics, their object, then the Start-skippable scene loop (the
 * title follows).
 * In:  the registers the scene routines pass through
 * Out: ends through the scene loop (the object's Scene_EndAfterDelay, or Start)
 * Evidence: lea Scr6_0489F4(pc),a6 / Scene_InitPlain / Scr6_048A86 VM6_Run / bsr.w
 *           Attract_SpawnRooftopHeroes / bra.w Scene_LoopSkippable; Tbl_00A152 entry 4; idle png
 *           f9000 / f16800
 */
void Attract_Rooftop(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2,
                     uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0;   /* not inputs (measured): the callees set them */
    uint16_t w1 = (uint16_t)*d1, w6 = (uint16_t)*d6;
    Scene_InitPlain(&d0, &w1, &d2, &w6, &d7, a2, a3, a4, Scr6_0489F4);
    set_low_word(d1, w1);
    set_low_word(d6, w6);
    *a6 = Scr6_048A86;
    /* the script runs to its end (Op6_End) and leaves its registers to the spawner */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);
    w1 = (uint16_t)*d1;
    Attract_SpawnRooftopHeroes(&d0, &w1, (uint16_t)*d4, (uint16_t)*d5, &d7, a2, a3, a4, *a6);
    set_low_word(d1, w1);
    dk::jump(at_Scene_LoopSkippable);
}
