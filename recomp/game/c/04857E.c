#include "game.h"

/* the searchlight scene's a6 scripts (ROM): the plain-scene header and the graphics */
static constexpr uint32_t Scr6_047CE2 = 0x047CE2u;
static constexpr uint32_t Scr6_047D76 = 0x047D76u;

enum {
    GATE_A_OFFSET = 0x120,   /* plane A's V-scroll = y minus this */
    GATE_B_FACTOR = 0x2400,  /* plane B's V-scroll = y * this / $10000 (0.14) */
    GATE_DRAW_FACTOR = 0x9A00, /* the sprites' y factor: muls takes it signed (-0.4, not 0.6) */
    GATE_DRAW_Y = 0x140,
    GATE_DRAW_X = 0x108,
    SCROLL_LINES = 0x0C,     /* g_ScrollMode: per-line H-scroll of both planes */
};

static uint32_t swap(uint32_t v) { return (v << 16) | (v >> 16); }
static uint32_t muls(uint16_t a, int16_t b) { return (uint32_t)((int32_t)(int16_t)a * b); }
static uint32_t with_low_word(uint32_t r, uint16_t v) { return (r & 0xFFFF0000u) | v; }
static void set_low_word(uint32_t *r, uint16_t v) { *r = with_low_word(*r, v); }
/* `add.w (g_ShakeY).w`: the integer part of the 16.16 shake */
static uint16_t shake_px(void) { return (uint16_t)((uint32_t)g_ShakeY >> 16); }

/* Story_GateScroll - native of the gate camera object: plane A's V-scroll = y - $120, plane B's
 * = y * 0.14 (both plus the shake): the vertical pan with parallax; and where its sprites are
 * drawn: x $108, y = y * $9A00 (signed: -0.4) / $10000 + $140.
 * In:  self = the camera object
 * Out: g_VScrollA/B; self->draw_x/draw_y; d0.w = plane A's scroll, d1 / d2 = the products
 *      swapped, low words plane B's scroll and the sprites' y
 * Evidence: move.w $42(a6),d0 / subi.w #$120 / muls.w #$2400,d1 / swap / add.w g_ShakeY to both /
 *           muls.w #$9A00,d2 / swap / addi.w #$140 / move.w #$108,$4E(a6); only caller
 *           Op2_CallNativeRel at $048562 in Scr2_04853A; scenario-ending 131416-144402
 */
void Story_GateScroll(uint16_t *d0, uint32_t *d1, uint32_t *d2, StoryCamObj *self)
{
    uint16_t y = (uint16_t)self->y;
    uint16_t shake = shake_px();
    *d0 = (uint16_t)(y - GATE_A_OFFSET + shake);
    uint32_t b = swap(muls(y, GATE_B_FACTOR));
    b = with_low_word(b, (uint16_t)(b + shake));
    *d1 = b;
    g_VScrollA = *d0;
    g_VScrollB = (uint16_t)b;
    uint32_t draw_y = swap(muls(y, (int16_t)GATE_DRAW_FACTOR));
    draw_y = with_low_word(draw_y, (uint16_t)(draw_y + GATE_DRAW_Y));
    *d2 = draw_y;
    self->draw_y = (int16_t)(uint16_t)draw_y;
    self->draw_x = GATE_DRAW_X;
}

/* Story_Searchlight - third story scene: a searchlight lamp in front of the moon. Plain scene, the
 * graphics, the lamp object, per-line scroll (mode $C, which Story_SearchlightScroll fills), then
 * the Start-skippable scene loop that keeps that mode.
 * In:  the registers the scene routines pass through
 * Out: ends through the scene loop (the object's Scene_EndAfterDelay $168, or Start)
 * Evidence: lea Scr6_047CE2(pc),a6 / Scene_InitPlain / Scr6_047D76 VM6_Run / bsr.w
 *           Story_SpawnSearchlightCamera / move.w #$C,g_ScrollMode / bra.w
 *           Scene_LoopSkippableKeepMode; Tbl_00A136 entry 3; idle png f3000 (the moon with a lamp)
 */
void Story_Searchlight(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2,
                       uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0;   /* not inputs (measured): the callees set them */
    uint16_t w1 = (uint16_t)*d1, w6 = (uint16_t)*d6;
    Scene_InitPlain(&d0, &w1, &d2, &w6, &d7, a2, a3, a4, Scr6_047CE2);
    set_low_word(d1, w1);
    set_low_word(d6, w6);
    *a6 = Scr6_047D76;
    /* the script runs to its end (Op6_End) and leaves its registers to the spawner */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);
    w1 = (uint16_t)*d1;
    Story_SpawnSearchlightCamera(&d0, &w1, (uint16_t)*d4, (uint16_t)*d5, &d7, a2, a3, a4, *a6);
    set_low_word(d1, w1);
    g_ScrollMode = SCROLL_LINES;
    dk::jump(at_Scene_LoopSkippableKeepMode);
}
