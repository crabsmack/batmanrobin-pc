#include "game.h"

enum {
    SWAY_PULL = 4,                 /* the wobble spring's pull toward 0 per frame (undamped) */
    CEILING_LINES = 0x58,          /* lines 0-87: the ceiling, spreading toward the back wall */
    WALL_LINES = 0x30,             /* 88-135: the back wall */
    FLOOR_LINES = 0x58,            /* 136-223: the floor, spreading back out */
    COLUMNS = 0x14,                /* 20 two-cell columns of plane A's V-scroll */
    SPREAD_DIVISOR = 0x92,         /* the slide's spread per line: slide / $92 */
    LINE_SHEAR = 0x70,             /* the wobble's shear: x $70 at the top line ... */
    COLUMN_SHEAR = 0xA8,           /* ... and x $A8 at the first column */
};
static constexpr int32_t SLIDE_ACCEL = 0x1000;          /* the slide speeds up by 1/16 px a frame ... */
static constexpr int32_t SLIDE_MAX_SPEED = -0x40000;    /* ... to 4 px a frame (leftward) */
static constexpr int32_t SLIDE_WRAP = 0x280000;         /* the pattern repeats every $50 px: kept within +-40 */
static constexpr int32_t SLIDE_PERIOD = 0x500000;
static constexpr int32_t CEILING_START = -0x200000;     /* the top line starts 32 px left */
static constexpr int32_t COLUMN_START = 0x100000;       /* the first column 16 px down */

static uint32_t swap(uint32_t v) { return (v << 16) | (v >> 16); }
static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* divs.w: quotient in the low word, remainder in the high word; on overflow the register stays */
static uint32_t divs_w(uint32_t d, int16_t s)
{
    int32_t q = (int32_t)d / s, r = (int32_t)d % s;
    if (q < -0x8000 || q > 0x7FFF)
        return d;
    return ((uint32_t)(uint16_t)r << 16) | (uint16_t)q;
}

/* Stage3_SpottedRoomScroll - per-frame scroll of the spotted room: swings the wobble spring
 * (g_Stage3RoomSway, +-4 a frame toward 0, undamped), slides the pattern (g_Stage3RoomSlide,
 * speeding up to -4.0, wrapped to +-40 px), then writes plane A's 224 line H-scrolls (88 ceiling
 * lines spreading by slide/$92 a line, 48 back-wall lines, 88 floor lines; all sheared by the
 * wobble) and 20 column V-scrolls (sheared by the wobble x 16 a column) into g_SceneWorkBuf
 * In:  the four g_Stage3Room* globals
 * Out: them; g_HScrollLinesA, g_SceneWorkBuf's 20 words; d1 = the wobble x 16 (long), d2 = the line
 *      shear after the last line, d3 = the column ramp after the last column, d4 = the floor's
 *      offset, d5 = the last line's word (swapped), d6 = the spread, a2/a3 past the tables
 * Evidence: move.w #4,d0 / bmi / neg.w / add.w into g_Stage3RoomSwayVel and g_Stage3RoomSway;
 *           g_Stage3RoomSlideVel cmp.l -$40000 / ble / sub.l $1000 / add.l g_Stage3RoomSlide /
 *           cmpi.l #$280000 / #$FFD80000 / -+$500000; move.w #$70,d2 / #$A8,d3 / muls.w by the
 *           wobble / neg.l / divs.w #$92 / ext.l / addi.l #$FFE00000 / the dbra loops of $57, $2F,
 *           $57, then asl.l #4,d1 / addi.l #$100000,d3 / $13; jsr from Stage3_RunSpottedRoom's
 *           frame loop; TAS f101686-f108293 (6476 frames)
 */
void Stage3_SpottedRoomScroll(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                              uint32_t *a2, uint32_t *a3)
{
    /* the wobble spring */
    int16_t pull = (int16_t)g_Stage3RoomSway < 0 ? SWAY_PULL : -SWAY_PULL;
    uint16_t sway_vel = (uint16_t)(g_Stage3RoomSwayVel + pull);
    g_Stage3RoomSwayVel = sway_vel;
    uint16_t sway = (uint16_t)(g_Stage3RoomSway + sway_vel);
    g_Stage3RoomSway = sway;
    set_low_word(d2, sway);

    /* the slide */
    int32_t slide_vel = (int32_t)g_Stage3RoomSlideVel;
    if (slide_vel > SLIDE_MAX_SPEED)
        slide_vel -= SLIDE_ACCEL;
    g_Stage3RoomSlideVel = (uint32_t)slide_vel;
    int32_t slide = (int32_t)((uint32_t)slide_vel + g_Stage3RoomSlide);
    if (slide >= SLIDE_WRAP)
        slide -= SLIDE_PERIOD;
    if (slide <= -SLIDE_WRAP)
        slide += SLIDE_PERIOD;
    g_Stage3RoomSlide = (uint32_t)slide;

    /* the shears: the wobble times the line / column factors (muls.w of the low words) */
    set_low_word(d2, LINE_SHEAR);
    *d3 = (uint32_t)SLIDE_MAX_SPEED;               /* d3 still holds the speed limit: move.w keeps its top */
    set_low_word(d3, COLUMN_SHEAR);
    int32_t wobble = (int16_t)g_Stage3RoomSway;
    *d1 = (uint32_t)wobble;
    *d3 = (uint32_t)-((int32_t)(int16_t)(uint16_t)*d3 * wobble);
    *d2 = (uint32_t)-((int32_t)(int16_t)(uint16_t)*d2 * wobble);
    *d4 = (uint32_t)slide;
    *d6 = (uint32_t)(int32_t)(int16_t)(uint16_t)divs_w(*d4, SPREAD_DIVISOR);
    *d4 += (uint32_t)CEILING_START;

    PTR(u16) line;
    line.v = dk::addr(g_HScrollLinesA);
    int n = 0;
    for (int i = 0; i < CEILING_LINES; i++, n++) {
        *d5 = swap(*d4 + *d2);
        line[n] = (uint16_t)*d5;
        *d2 += *d1;
        *d4 -= *d6;
    }
    for (int i = 0; i < WALL_LINES; i++, n++) {
        *d5 = swap(*d4 + *d2);
        line[n] = (uint16_t)*d5;
        *d2 += *d1;
    }
    for (int i = 0; i < FLOOR_LINES; i++, n++) {
        *d5 = swap(*d4 + *d2);
        line[n] = (uint16_t)*d5;
        *d2 += *d1;
        *d4 += *d6;
    }
    *a2 = line.v + 2 * n;

    *d1 <<= 4;
    *d3 += (uint32_t)COLUMN_START;
    PTR(u16) column;
    column.v = dk::addr(g_SceneWorkBuf);
    for (int i = 0; i < COLUMNS; i++) {
        column[i] = (uint16_t)(*d3 >> 16);
        *d3 += *d1;
    }
    *a3 = column.v + 2 * COLUMNS;
}
