#include "game.h"

/* the monitors scene's a6 scripts (ROM): the plain-scene header and the graphics */
static constexpr uint32_t Scr6_047D18 = 0x047D18u;
static constexpr uint32_t Scr6_047E32 = 0x047E32u;

enum {
    FACADE_A_OFFSET = 0x68,    /* plane A's V-scroll = y minus this */
    FACADE_B_FACTOR = 0x4000,  /* plane B's = y / 4 */
};

static uint32_t swap(uint32_t v) { return (v << 16) | (v >> 16); }
static uint32_t with_low_word(uint32_t r, uint16_t v) { return (r & 0xFFFF0000u) | v; }
static void set_low_word(uint32_t *r, uint16_t v) { *r = with_low_word(*r, v); }

/* Story_FacadeScroll - native of the facade camera: plane A's V-scroll = y - $68 + the shake,
 * plane B's = y / 4 + half the shake.
 * In:  self = the camera object
 * Out: g_VScrollA/B; d0.w = plane A's, d1 = the swapped product with plane B's in its low word,
 *      d2.w = half the shake
 * Evidence: move.w $42(a6),d0 / subi.w #$68 / muls.w #$4000,d1 / swap / g_ShakeY added, asr.w #1
 *           for plane B; only caller Op2_CallNativeRel at $0487F6 in Scr2_0487DA;
 *           scenario-ending 141778-145772
 */
void Story_FacadeScroll(uint16_t *d0, uint32_t *d1, uint16_t *d2, const StoryCamObj *self)
{
    uint16_t y = (uint16_t)self->y;
    uint32_t b = swap((uint32_t)((int32_t)(int16_t)y * FACADE_B_FACTOR));
    uint16_t shake = (uint16_t)((uint32_t)g_ShakeY >> 16);   /* the 16.16 shake's integer part */
    *d0 = (uint16_t)(y - FACADE_A_OFFSET + shake);
    *d2 = (uint16_t)((int16_t)shake >> 1);
    b = with_low_word(b, (uint16_t)(b + *d2));
    *d1 = b;
    g_VScrollA = *d0;
    g_VScrollB = (uint16_t)b;
}

/* Story_Monitors - second story scene: the asylum's four security monitors (static, then the
 * ALARM panel). Plain scene, the graphics, the camera object, then the Start-skippable loop.
 * In:  the registers the scene routines pass through
 * Out: ends through the scene loop (the object's Scene_EndAfterDelay $78, or Start)
 * Evidence: lea Scr6_047D18(pc),a6 / Scene_InitPlain / Scr6_047E32 VM6_Run / bsr.w
 *           Story_SpawnMonitorsCamera / bra.w Scene_LoopSkippable; Tbl_00A136 entry 2; idle png
 *           f2400, scenario-ending png f141000 (ALARM)
 */
void Story_Monitors(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2,
                    uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0;   /* not inputs (measured): the callees set them */
    uint16_t w1 = (uint16_t)*d1, w6 = (uint16_t)*d6;
    Scene_InitPlain(&d0, &w1, &d2, &w6, &d7, a2, a3, a4, Scr6_047D18);
    set_low_word(d1, w1);
    set_low_word(d6, w6);
    *a6 = Scr6_047E32;
    /* the script runs to its end (Op6_End) and leaves its registers to the spawner */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);
    w1 = (uint16_t)*d1;
    Story_SpawnMonitorsCamera(&d0, &w1, (uint16_t)*d4, (uint16_t)*d5, &d7, a2, a3, a4, *a6);
    set_low_word(d1, w1);
    dk::jump(at_Scene_LoopSkippable);
}
