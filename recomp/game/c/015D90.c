#include "game.h"

/* Tbl_059B22: the sine table, a signed word per even angle (index & $3FE as a byte offset) */
static constexpr uint32_t Tbl_059B22 = 0x059B22u;

/* the facade's five bands of plane B lines, top to bottom: the lines each fills and the divisor of
   the segment offset that gives its per-line shear */
enum {
    FACADE_BAND1_LINES = 0x58,    /* minus the view y: the band starts above the screen */
    FACADE_BAND2_LINES = 0x10,
    FACADE_BAND3_LINES = 0x08,
    FACADE_BAND4_LINES = 0x40,
    FACADE_BAND5_LINES = 0x30,    /* plus the view y */
    FACADE_BAND1_DIV = 0x3720,
    FACADE_BAND2_DIV = 0x14A0,
    FACADE_BAND3_DIV = 0x1C20,
    FACADE_BAND5_DIV = 0x1950,
    FACADE_BAND4_SKIP = 0x18,     /* band 4 restarts band 2's ramp this many lines on */
};
static constexpr uint32_t FACADE_BAND1_START = 0xFF508000u;   /* -$AF.8 px: band 1's first line */

/* the fire's windows: the upper-floor window x in a $168-px segment for a remainder up to each limit */
static constexpr uint16_t FIRE_SEGMENT = 0x168;
static const struct { uint16_t limit, window; } FIRE_WINDOWS[] = {
    { 0x48, 0x18 }, { 0x80, 0x50 }, { 0xC0, 0x88 }, { 0xF8, 0xD0 },
};
static constexpr uint16_t FIRE_LAST_WINDOW = 0x108;
static constexpr uint8_t FIRE_STARTING = 2;   /* g_Stage1FireState: blank plane A's top, then burn */

static constexpr int32_t FIRE_OFFSCREEN = (int32_t)0xFF600000;  /* fire x - facade x at or below -$A0: off screen */
static constexpr uint16_t FIRE_LINES = 0x50;                    /* the fire rows: lines 0..$4F (minus the view y) */
static constexpr int32_t FIRE_SPAN_DIV = 0x3720;
static constexpr int16_t FIRE_SPAN_SCALE = 0x500;
static constexpr int32_t FIRE_LEAN_BIAS = 0x2000;
static constexpr uint32_t ROUND_HALF = 0x8000u;

static constexpr uint16_t SKY_LINES = 0x2F;      /* the sky band: lines 0..$2F (minus the view y) */
static constexpr uint16_t SKY_PHASE_STEP = 0x18; /* the ripple's phase step per line */

/* divs.w: quotient in the low word, remainder in the high word; on overflow the register stays */
static uint32_t divs_w(uint32_t d, int16_t s)
{
    int32_t q = (int32_t)d / s, r = (int32_t)d % s;
    if (q < -32768 || q > 32767) return d;
    return ((uint32_t)(uint16_t)r << 16) | (uint16_t)q;
}

/* divs.w / ext.l / asl.l #4: a band's per-line step from the segment offset (16.16) */
static uint32_t band_slope(uint32_t offset, int16_t divisor)
{
    return (uint32_t)(int32_t)(int16_t)divs_w(offset, divisor) << 4;
}

static uint16_t view_y(void) { return (uint16_t)(g_ViewY >> 16); }

/* Stage1_ScrollFacadeLines - fills the plane B line scroll of the stage 1 street in five bands whose
 * shear depends on the offset in the facade segment, so the building's parts scroll as if at
 * different depths
 * In:  g_Stage1SegOfs (16.16), g_ViewY; d3 / a1 = the second ramp Mem_FillRamp also writes (the
 *      caller's registers: a bug of the original, every band writes there too)
 * Out: g_HScrollLinesB's lines; d1 = band 5's step, d2 = the offset advanced by the second ramp,
 *      d3 = band 2's start, d4 = band 1's step x $18, d6 low word = $FFFF
 * Evidence: move.l g_Stage1SegOfs,d2 / divs.w #$3720 / ext.l / asl.l #4 / neg.l; d0 = d2 + $FF508000
 *           advanced ViewY lines (bra into dbra d6); then five jsr/jmp Mem_FillRamp with d7 = $58 -
 *           ViewY, $10, 8, $40, $30 + ViewY; muls.w #$18,d4; only caller Stage1_UpdateStreetScroll.
 *           The last jmp Mem_FillRamp is a call here (Mem_FillRamp's run ends in its rts)
 */
void Stage1_ScrollFacadeLines(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint16_t *d6, uint32_t a1)
{
    uint32_t a0 = ADDR68(g_HScrollLinesB);
    *d2 = g_Stage1SegOfs;
    *d1 = 0u - band_slope(*d2, FACADE_BAND1_DIV);
    uint32_t d0 = *d2 + FACADE_BAND1_START;
    uint16_t lines = (uint16_t)(FACADE_BAND1_LINES - view_y());
    *d6 = view_y();
    while (--*d6 != 0xFFFF)            /* the band starts ViewY lines above the screen */
        d0 += *d1;
    Mem_FillRamp(&d0, *d1, d2, *d3, lines, &a0, &a1);
    *d3 = d0;                          /* band 2's start and band 1's step, for band 4 */
    *d4 = *d1;
    *d1 = band_slope(*d2, FACADE_BAND2_DIV);
    Mem_FillRamp(&d0, *d1, d2, *d3, FACADE_BAND2_LINES, &a0, &a1);
    d0 -= *d1;
    *d1 = 0u - band_slope(*d2, FACADE_BAND3_DIV);
    Mem_FillRamp(&d0, *d1, d2, *d3, FACADE_BAND3_LINES, &a0, &a1);
    d0 = *d3;
    *d1 = *d4;
    *d4 = (uint32_t)((int32_t)(int16_t)*d4 * FACADE_BAND4_SKIP);   /* muls.w */
    d0 += *d4;
    Mem_FillRamp(&d0, *d1, d2, *d3, FACADE_BAND4_LINES, &a0, &a1);
    *d1 = band_slope(*d2, FACADE_BAND5_DIV);
    Mem_FillRamp(&d0, *d1, d2, *d3, (uint16_t)(FACADE_BAND5_LINES + view_y()), &a0, &a1);
}

/* Stage1_StartFireAt - trigger callback: sets the stage 1 building fire at the upper-floor window
 * nearest a world x and starts it if none burns
 * In:  d4 = world x
 * Out: g_Stage1FireX integer part = x * 35/32 snapped to the window $18/$50/$88/$D0/$108 of its
 *      $168 segment; g_Stage1FireState = 2 when it was 0; d0 = 0 (Z set: the trigger is done);
 *      d1 = segment << 16 | the remainder; d2 low word = the window
 * Evidence: moveq #0,d0 / move.w d4,d0 / x + x/8 - x/32 / divu.w #$168 / swap d1 / sub.w d1,d0 / the
 *           ble ladder $48/$80/$C0/$F8 / add.w d2,d0 / move.w d0,g_Stage1FireX / tst.b / move.b #2 /
 *           moveq #0,d0; callback longs in the street trigger table Tbl_015F5A ($16158-$16182) and
 *           $20D98; the fire appears over the bank in the TAS from f2160
 */
void Stage1_StartFireAt(uint32_t *d0, uint32_t *d1, uint16_t *d2, uint16_t d4)
{
    uint32_t x = d4;
    uint32_t eighth = x >> 3;
    uint32_t facade = x + eighth - (eighth >> 2);    /* x * 35/32 with the asm's truncations */
    uint16_t rem = (uint16_t)((uint16_t)facade % FIRE_SEGMENT);
    *d1 = ((uint32_t)((uint16_t)facade / FIRE_SEGMENT) << 16) | rem;   /* divu.w / swap */
    uint16_t at = (uint16_t)((uint16_t)facade - rem);
    *d2 = FIRE_LAST_WINDOW;
    for (const auto &w : FIRE_WINDOWS) {
        if ((int16_t)rem <= (int16_t)w.limit) {
            *d2 = w.window;
            break;
        }
    }
    at = (uint16_t)(at + *d2);
    g_Stage1FireX = (g_Stage1FireX & 0xFFFFu) | ((uint32_t)at << 16);
    if (g_Stage1FireState == 0)
        g_Stage1FireState = FIRE_STARTING;
    *d0 = 0;
}

/* Stage1_SwayFireLines - sways the stage 1 building fire: writes the plane A line scroll of the fire
 * rows from the bottom up with a sine-driven lean, when the fire is on screen
 * In:  g_Stage1FireX, the facade x and the fire frame's x offset (Stage1StreetVars), g_ViewY
 * Out: g_HScrollLinesA lines 0..$4F - ViewY (written upward); g_Stage1FirePhase += 2; d1 = the lean
 *      per line (sine - $2000), d4 = the span x $500 (both unchanged when off screen)
 * Evidence: move.l g_Stage1FireX,d0 / sub.l $F506,d0 / cmpi.l #$FF600000 / ble rts / divs.w #$3720 /
 *           ext.l / muls.w #$500,d4 / sub.l d4,d0 / addq.w #2,g_Stage1FirePhase / andi.w #$3FE /
 *           Tbl_059B22 / subi.l #$2000 / addi.l #$8000,d0 / the frame x offset into d0's high word /
 *           move.w d0,-(a0) per line, dbra; branch from Stage1_UpdateStreetScroll when the state > 0
 *           (TAS f2160-f4518)
 */
void Stage1_SwayFireLines(uint32_t *d1, uint32_t *d4)
{
    const Stage1StreetVars *vars = (const Stage1StreetVars *)g_SceneVars;
    uint32_t x = g_Stage1FireX - vars->facade_x;
    if ((int32_t)x <= FIRE_OFFSCREEN)
        return;
    *d1 = (uint32_t)(int32_t)(int16_t)divs_w(x, FIRE_SPAN_DIV);
    *d4 = (uint32_t)((int32_t)(int16_t)*d1 * FIRE_SPAN_SCALE);
    x -= *d4;
    g_Stage1FirePhase += 2;
    PTR(const s16) sine;
    sine.v = Tbl_059B22;
    *d1 = (uint32_t)((int32_t)sine.get()[(g_Stage1FirePhase & 0x3FE) / 2] - FIRE_LEAN_BIAS);
    x += ROUND_HALF;
    x += (uint32_t)vars->fire_frame_x << 16;   /* add.w on the swapped high word */
    uint16_t lines = (uint16_t)(FIRE_LINES - view_y());
    u16 *line = (u16 *)g_HScrollLinesA + (int16_t)lines;   /* the rows are written upward from here */
    uint16_t n = (uint16_t)(lines - 1);
    do {
        *--line = (uint16_t)(x >> 16);
        x += *d1;
    } while (n-- != 0);
}

/* Stage1_WaveSkyLines - before any fire: ripples the top band of plane A (above the stage 1 street's
 * building) with a sine wave that drifts each frame
 * In:  the facade x (Stage1StreetVars), g_ViewY, g_FrameParity
 * Out: g_HScrollLinesA lines 0..$2F - ViewY = sine/4 - facade x (bit 0 set on odd frames, clear on
 *      even); g_Stage1SkyPhase += 2; d1 low word = the phase after the last line; d6 = ViewY x $18
 * Evidence: addq.w #2,g_Stage1SkyPhase / move.w $F506,d2 / d7 = $2F - ViewY / mulu.w #$18,d6 / add.w
 *           d6,d1 / per line: andi.w #$3FE,d1 / move.b (a0,d1.w),d0 (the sine word's high byte) /
 *           ext.w / asr.w #2 / sub.w d2,d0 / andi.w #$FFFE or ori.w #1 / addi.w #$18,d1; branch from
 *           Stage1_UpdateStreetScroll when the state = 0 (TAS f532-f2160)
 */
void Stage1_WaveSkyLines(uint16_t *d1, uint32_t *d6)
{
    const Stage1StreetVars *vars = (const Stage1StreetVars *)g_SceneVars;
    PTR(const s16) sine;
    sine.v = Tbl_059B22;
    g_Stage1SkyPhase += 2;
    uint16_t phase = g_Stage1SkyPhase;
    uint16_t facade = (uint16_t)(vars->facade_x >> 16);
    uint16_t n = (uint16_t)(SKY_LINES - view_y());
    *d6 = (uint32_t)view_y() * SKY_PHASE_STEP;   /* mulu.w: the band starts ViewY lines down the wave */
    phase = (uint16_t)(phase + *d6);
    bool odd = g_FrameParity != 0;
    u16 *line = (u16 *)g_HScrollLinesA;
    do {
        phase &= 0x3FE;
        int16_t wave = (int16_t)(int8_t)((uint16_t)sine.get()[phase / 2] >> 8) >> 2;
        uint16_t v = (uint16_t)(wave - facade);
        *line++ = odd ? (uint16_t)(v | 1) : (uint16_t)(v & 0xFFFE);
        phase = (uint16_t)(phase + SKY_PHASE_STEP);
    } while (n-- != 0);
    *d1 = phase;
}
