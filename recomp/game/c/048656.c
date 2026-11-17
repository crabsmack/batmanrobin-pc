#include "game.h"

/* the guard tower scene's a6 scripts (ROM): the plain-scene header and the graphics */
static constexpr uint32_t Scr6_047CF4 = 0x047CF4u;
static constexpr uint32_t Scr6_047DC2 = 0x047DC2u;
/* Tbl_059B22: the sine table, a signed word per even index (its high byte read here: 1.0 = $7F) */
static constexpr uint32_t Tbl_059B22 = 0x059B22u;

enum {
    SCREEN_LINES = 224,
    PLANE_B_FACTOR = 0x3000,   /* plane B's lines = x * this / $10000 (3/16) */
    PLANE_A_FACTOR = 0x4000,   /* plane A's base = x / 4 */
    PHASE_STEP = 2,            /* the wobble's phase per frame */
    DRIFT_STEP = 0x6000,       /* plane A's drift per frame (16.16) */
    PHASE_PER_LINE = 0x0A,
    PHASE_MASK = 0x3FE,        /* even byte offsets into the 512-word sine table */
};

static uint32_t swap(uint32_t v) { return (v << 16) | (v >> 16); }
static uint32_t muls(uint16_t a, int16_t b) { return (uint32_t)((int32_t)(int16_t)a * b); }
static uint32_t with_low_word(uint32_t r, uint16_t v) { return (r & 0xFFFF0000u) | v; }
static void set_low_word(uint32_t *r, uint16_t v) { *r = with_low_word(*r, v); }

/* the sine table's byte at an even offset: the high byte of that entry */
static int8_t sine_byte(uint16_t offset)
{
    PTR(const u8) p;
    p.v = Tbl_059B22 + offset;
    return (int8_t)*p.get();
}

/* Story_SearchlightScroll - native of the searchlight scene's object: every plane B line at
 * x * 3/16; plane A's lines at x / 4 plus a drift (+$6000 a frame) plus a sine wobble (sine / 8,
 * its phase +2 a frame plus the caller's d2, +$A per line).
 * In:  self = the object; d2.w = the phase base from the caller; d3 = Mem_FillWords' pass-through
 * Out: g_HScrollLinesA/B; self->counter += 2, self->drift += $6000; d0 = the products' leftovers
 *      with the last line's value, d1 = plane A's base (swapped product, drift added), d2.w = the
 *      phase after the last line, d7.w = -1, a1 = past g_HScrollLinesA's 224 words
 * Evidence: muls.w #$4000,d1 / muls.w #$3000,d0 / swap / Mem_FillWords $E0 into g_HScrollLinesB /
 *           addq.w #2,$2A(a6) / addi.l #$6000,$46(a6) / the 224-line loop over Tbl_059B22 (andi.w
 *           #$3FE, ext.w, asr.w #3); only caller Op2_CallNativeRel at $048610 in Scr2_0485E4;
 *           scenario-ending 141217-145528
 */
void Story_SearchlightScroll(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint16_t *d7, uint32_t *a1,
                             StoryCamObj *self)
{
    uint16_t x = (uint16_t)self->x;
    uint32_t base = swap(muls(x, PLANE_A_FACTOR));
    uint32_t value = swap(muls(x, PLANE_B_FACTOR));
    uint32_t a0 = dk::addr(g_HScrollLinesB);
    *d7 = SCREEN_LINES;
    Mem_FillWords(&value, base, d2, d3, d7, &a0, a1);

    self->counter += PHASE_STEP;
    uint16_t phase = (uint16_t)((uint16_t)*d2 + self->counter);
    self->drift += DRIFT_STEP;
    base = with_low_word(base, (uint16_t)(base + (uint16_t)((uint32_t)self->drift >> 16)));
    u16 *line = (u16 *)g_HScrollLinesA;
    for (int i = 0; i < SCREEN_LINES; i++) {
        phase &= PHASE_MASK;
        uint16_t v = (uint16_t)((int16_t)sine_byte(phase) >> 3);
        v = (uint16_t)(v + base);
        line[i] = v;
        value = with_low_word(value, v);
        phase += PHASE_PER_LINE;
    }
    *d0 = value;
    *d1 = base;
    set_low_word(d2, phase);
    *d7 = 0xFFFF;   /* the dbra count run out */
    *a1 = dk::addr(line + SCREEN_LINES);
}

/* Story_GuardTower - fourth story scene: the asylum guard tower with its searchlight beam. Plain
 * scene, the graphics, the object that drops in along the tower, then the Start-skippable loop.
 * In:  the registers the scene routines pass through
 * Out: ends through the scene loop (the object's Scene_EndAfterDelayWhite, or Start)
 * Evidence: lea Scr6_047CF4(pc),a6 / Scene_InitPlain / Scr6_047DC2 VM6_Run / bsr.w
 *           Story_SpawnTowerCamera / bra.w Scene_LoopSkippable; Tbl_00A136 entry 4; idle png f7200
 *           (the tower and the beam)
 */
void Story_GuardTower(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2,
                      uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0;   /* not inputs (measured): the callees set them */
    uint16_t w1 = (uint16_t)*d1, w6 = (uint16_t)*d6;
    Scene_InitPlain(&d0, &w1, &d2, &w6, &d7, a2, a3, a4, Scr6_047CF4);
    set_low_word(d1, w1);
    set_low_word(d6, w6);
    *a6 = Scr6_047DC2;
    /* the script runs to its end (Op6_End) and leaves its registers to the spawner */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);
    w1 = (uint16_t)*d1;
    Story_SpawnTowerCamera(&d0, &w1, (uint16_t)*d4, (uint16_t)*d5, &d7, a2, a3, a4, *a6);
    set_low_word(d1, w1);
    dk::jump(at_Scene_LoopSkippable);
}
