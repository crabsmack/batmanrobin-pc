#include "game.h"

/* the sine table (a word per 2 units of angle; read here by its high bytes) */
static constexpr uint32_t Tbl_059B22 = 0x059B22u;

enum {
    HALL_FLOOR_FIRST_LINE = 160,   /* plane B's floor lines: 160-223 */
    HALL_FLOOR_PAIRS = 32,         /* two lines per step, one per phase */
    HALL_RIPPLE_STEP = 0x0E,       /* each phase advances this much per line */
    HALL_ANGLE_MASK = 0x3FE,       /* even byte offsets 0-$3FE into the table */
};

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Stage4_HallFloorRipple - the hall's scroll per frame: plane B's V-scroll = g_ViewY * 7/8 (three
 * halvings summed) and the 64 plane-B lines 160-223 (the floor) shifted by -camera x plus a sine
 * ripple / 4 from two phases, alternating line by line (+1 and +2 a frame; the second also + a
 * drift; the two swap lines on odd frames)
 * In:  g_ViewY, g_CameraX (integer words), g_SceneVars (HallScrollVars), g_FrameParity
 * Out: g_VScrollB, g_HScrollLinesB lines 160-223; d1 / d2 = the two phases as the loop leaves them
 *      (exchanged whole on odd frames; d2's low word first holds the V-scroll), d3 = camera x, d4 =
 *      $E, d5 = $3FE
 * Evidence: move.w g_ViewY,d0 / asr.w #1 / d2 = d0 / asr / add / asr / add / g_VScrollB / lea
 *           g_HScrollLinesB+$140 / addq.w #1,+4 / addq.w #2,+6 / addq.w #1,+0 / add.w +0,d2 / tst.b
 *           g_FrameParity / exg.l d1,d2 / the dbra of 32: and.w d5 / move.b (a0,d1.w) / ext.w / asr.w
 *           #2 / sub.w d3 / add.w d4; called every frame by Stage4_RunHall; TAS 116192-119385
 */
void Stage4_HallFloorRipple(uint32_t *d1, uint32_t *d2, uint16_t *d3, uint16_t *d4, uint16_t *d5)
{
    int16_t view_y = (int16_t)(uint16_t)((uint32_t)g_ViewY >> 16);
    int16_t v = (int16_t)(view_y >> 1);
    int16_t scroll = v;
    v = (int16_t)(v >> 1);
    scroll = (int16_t)(scroll + v);
    v = (int16_t)(v >> 1);
    scroll = (int16_t)(scroll + v);
    set_low_word(d2, (uint16_t)scroll);
    g_VScrollB = (uint16_t)scroll;

    PTR(const s8) sine;
    sine.v = Tbl_059B22;
    const s8 *table = sine.get();
    s16 *line = (s16 *)g_HScrollLinesB + HALL_FLOOR_FIRST_LINE;
    HallScrollVars *vars = (HallScrollVars *)g_SceneVars;
    vars->phase_a += 1;
    vars->phase_b += 2;
    set_low_word(d1, vars->phase_a);
    set_low_word(d2, vars->phase_b);
    vars->drift += 1;
    set_low_word(d2, (uint16_t)(*d2 + vars->drift));
    *d3 = (uint16_t)((uint32_t)g_CameraX >> 16);
    *d4 = HALL_RIPPLE_STEP;
    *d5 = HALL_ANGLE_MASK;
    if (g_FrameParity) {
        uint32_t t = *d1;
        *d1 = *d2;
        *d2 = t;
    }
    for (int i = 0; i < HALL_FLOOR_PAIRS; i++) {
        uint32_t *phase[2] = { d1, d2 };
        for (uint32_t *p : phase) {
            set_low_word(p, (uint16_t)(*p & *d5));
            int16_t ripple = (int16_t)(table[(uint16_t)*p] >> 2);
            *line++ = (int16_t)(ripple - (int16_t)*d3);
            set_low_word(p, (uint16_t)(*p + *d4));
        }
    }
}
