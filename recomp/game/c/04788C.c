#include "game.h"

/* Tbl_059B22: the sine table, a signed word per even index (its high byte read here: 1.0 = $7F) */
static constexpr uint32_t Tbl_059B22 = 0x059B22u;

enum {
    UPPER_TOP = 24,               /* the upper wall's lines: from 24 down ... */
    UPPER_BOTTOM = 120,           /* ... and from 119 up, meeting in the middle */
    UPPER_HALF_START = 0x8000,    /* the top line's distance term (16.16: half a pixel) */
    UPPER_BASE_OFFSET = 0x1200000,
    UPPER_DIVISOR = 0x120,
    UPPER_FAR_FACTOR = 0x5F,
    UPPER_TERM_START = 0x17E,     /* the stepping's second difference */
    PLANE_B_WOBBLE_FIRST = 32,    /* plane B's wobbling lines: 32-119 */
    PLANE_B_WOBBLE_LINES = 88,
    WOBBLE_STEP = 0x0A,           /* the phase per frame */
    WOBBLE_PER_LINE = 0x10,
    PHASE_MASK = 0x3FE,
    PLANE_B_DRIFT_FACTOR = 0x6580,  /* plane B's lines move by floor_x * this / $10000 (0.4) */
    PLANE_B_FRONT_FIRST = 176,    /* the foreground lines: 176-223, at -1.25 * g_CameraX */
    PLANE_B_FRONT_COUNT_MINUS1 = 0x2F,
};

static uint32_t swap(uint32_t v) { return (v << 16) | (v >> 16); }
static uint32_t with_low_word(uint32_t r, uint16_t v) { return (r & 0xFFFF0000u) | v; }

/* divu.w: quotient in the low word, remainder in the high word; on overflow the register stays */
static uint32_t divu_w(uint32_t d, uint16_t s)
{
    uint32_t q = d / s, r = d % s;
    if (q > 0xFFFFu) return d;
    return (r << 16) | q;
}

/* the 16.16 shake's integer part (`move.w (g_ShakeY).w`) */
static int16_t shake_px(void) { return (int16_t)(uint16_t)((uint32_t)g_ShakeY >> 16); }

/* the sine table's byte at an even offset: the high byte of that entry */
static int8_t sine_byte(uint16_t offset)
{
    PTR(const u8) p;
    p.v = Tbl_059B22 + offset;
    return (int8_t)*p.get();
}

/* Stage4_LabUpperLines - plane A H-scroll of the lab's upper wall: lines from 24 down and from 119
 * up meet in the middle, each a perspective value from the wall's base (vars->upper) by
 * incremental quadratic stepping: a top line for every few bottom lines.
 * In:  g_SceneVars (LabScrollVars: upper), g_ShakeY
 * Out: g_HScrollLinesA lines 24-119 (minus the shake); d3 = $FFFF:the last first difference,
 *      d4.w = the last second difference, d5 = -(the per-line step), d6 = the last bottom value
 *      (halves swapped), a2 / a3 = past the last line written from each end
 * Evidence: lea g_HScrollLinesA+$30,a2 / +$F0,a3 / suba.w 2*shake / move.l #$8000,d0 / subi.l
 *           #$1200000,d7 / neg.l / divu.w #$120 / mulu.w #$5F / the d2/d3/d4 step loop (bpl, ble);
 *           only caller Stage4_LabScroll; TAS 119416-122651
 */
void Stage4_LabUpperLines(uint32_t *d3, uint16_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a2, uint32_t *a3)
{
    const LabScrollVars *vars = (const LabScrollVars *)g_SceneVars;
    int16_t shake = shake_px();
    u16 *top = (u16 *)g_HScrollLinesA + UPPER_TOP - shake;
    u16 *bottom = (u16 *)g_HScrollLinesA + UPPER_BOTTOM - shake;

    uint32_t near = UPPER_HALF_START;                     /* d0: the top line's term */
    uint32_t base = (uint32_t)(int32_t)vars->upper;
    uint32_t origin = base - UPPER_BASE_OFFSET;           /* d7 */
    uint32_t q = divu_w(0u - base, UPPER_DIVISOR);
    uint32_t far = 0u - (uint32_t)(uint16_t)q * UPPER_FAR_FACTOR;   /* d1: the bottom line's term */
    uint32_t step = 0u - (q & 0xFFFFu);                   /* d5 */
    uint16_t acc = 0;                                      /* d2 */
    uint32_t diff = 0xFFFFFFFEu;                           /* d3: moveq #-2 */
    uint16_t term = UPPER_TERM_START;                      /* d4 */
    uint32_t value = *d6;

    *top++ = (uint16_t)((origin - near) >> 16);
    for (;;) {
        value = swap(origin - far);
        *--bottom = (uint16_t)value;
        near += step;
        diff = with_low_word(diff, (uint16_t)(diff + 4));
        acc = (uint16_t)(acc - (uint16_t)diff);
        if ((int16_t)acc >= 0)
            continue;                                      /* bpl: another bottom line */
        acc = (uint16_t)(acc + term);
        term = (uint16_t)(term - 4);
        far -= step;
        if ((int32_t)far > (int32_t)near)
            break;                                         /* the two ends have met */
        *top++ = (uint16_t)((origin - near) >> 16);
    }
    *d3 = diff;
    *d4 = term;
    *d5 = step;
    *d6 = value;
    *a2 = dk::addr(top);
    *a3 = dk::addr(bottom);
}

/* Stage4_LabPlaneBLines - plane B H-scroll of the lab: lines 32-119 a sine wobble (/64, its phase
 * +$A a frame and +$10 per line) minus floor_x * 0.4; then 48 lines from 176 (moved up with the
 * shake) at -1.25 * g_CameraX: the foreground.
 * In:  g_SceneVars (LabScrollVars: floor_x, wobble), g_CameraX, g_ShakeY
 * Out: g_HScrollLinesB lines 32-119 and (176-223) - shake; vars->wobble += $A; d1 = g_CameraX / 4
 *      (asr.l), d2 = floor_x's pixels * $6580 with its halves swapped
 * Evidence: lea g_HScrollLinesB+$40,a1 / addi.w #$A,g_SceneVars+$E / muls.w #$6580 / swap / 88 x
 *           (andi.w #$3FE, ext.w, asr.w #6, sub.w d2) / lea g_HScrollLinesB+$160 / moveq #$2F +
 *           shake / asr.l #2 / add.l / swap / neg.w; reached by bra.w from Stage4_LabScroll
 */
void Stage4_LabPlaneBLines(uint32_t *d1, uint32_t *d2)
{
    LabScrollVars *vars = (LabScrollVars *)g_SceneVars;
    u16 *line = (u16 *)g_HScrollLinesB + PLANE_B_WOBBLE_FIRST;
    vars->wobble += WOBBLE_STEP;
    uint16_t phase = vars->wobble;
    uint16_t floor_px = (uint16_t)((uint32_t)vars->floor_x >> 16);
    *d2 = swap((uint32_t)((int32_t)(int16_t)floor_px * PLANE_B_DRIFT_FACTOR));
    for (int i = 0; i < PLANE_B_WOBBLE_LINES; i++) {
        phase &= PHASE_MASK;
        *line++ = (uint16_t)(((int16_t)sine_byte(phase) >> 6) - (uint16_t)*d2);
        phase += WOBBLE_PER_LINE;
    }

    int16_t shake = shake_px();
    line = (u16 *)g_HScrollLinesB + PLANE_B_FRONT_FIRST - shake;
    uint16_t count_minus1 = (uint16_t)(PLANE_B_FRONT_COUNT_MINUS1 + shake);
    uint32_t cam = (uint32_t)g_CameraX;
    *d1 = (uint32_t)((int32_t)cam >> 2);
    uint16_t front = (uint16_t)-(uint16_t)((cam + *d1) >> 16);
    for (uint32_t n = (uint32_t)count_minus1 + 1; n != 0; n--)   /* dbra d7 */
        *line++ = front;
}
