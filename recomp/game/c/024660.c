#include "game.h"

/* the corridor's 32 middle-wall band rates (ROM longs, added each frame) */
static constexpr uint32_t Tbl_024880 = 0x024880u;

enum {
    ARENA_LINES = 0xE0,               /* 224 screen lines */
    ARENA_COLUMNS = 0x14,             /* 20 two-cell columns */
    ARENA_FLOOR_SURFACE_Y = 0xC4,     /* the floor's screen y at the pivot */
    ARENA_SPLIT_MIN = 0x40,           /* the split line never above line 64 */
    ARENA_SPLIT_BIAS = 0x86,
    ARENA_SPLIT_NONE_LINE = 0x80,     /* the hull out of view: the split parked at line 128 */
    ARENA_NO_PICTURE_HSCROLL = 0xFE00,
    CORRIDOR_BANDS = 32,
    CORRIDOR_STRIPE = 0xA000,         /* the floor/ceiling stripe period in g_MapScrollX units */
};
static constexpr uint32_t ARENA_LINE_BIAS = 0x80000u;          /* 8 px (16.16) */
static constexpr uint32_t ARENA_LOWER_BIAS = 0xFD500000u;      /* -$2B0 px (16.16) */
static constexpr uint32_t ARENA_ALT_HALF = 0x2000000u;         /* $200 px: the plane's other half */
static constexpr int32_t ARENA_SHOW_PICTURE_UP = 0x9E0000;     /* the hull's lowest visible offset */
static constexpr int32_t ARENA_SHOW_PICTURE_DOWN = 0x850000;   /* the same with the floor tilted left */
static constexpr uint32_t ARENA_GREY_COLUMNS = 0x800080u;      /* two columns at V-scroll $80 */
static constexpr uint32_t CORRIDOR_Y_BIAS = 0xE80000u;         /* $E8 px (16.16) */

/* muls.w: the low words, signed, into a long */
static uint32_t Tilt_Muls(uint32_t a, uint32_t b)
{
    return (uint32_t)((int32_t)(int16_t)a * (int32_t)(int16_t)b);
}

/* a register's low word replaced (move.w / add.w into a data register) */
static uint32_t Tilt_Low(uint32_t r, uint16_t w)
{
    return (r & 0xFFFF0000u) | w;
}

/* swap: the 68000's halves exchanged */
static uint32_t Tilt_Swap(uint32_t v)
{
    return (v << 16) | (v >> 16);
}

/* the shake's integer word (the long's high word, read as a word) */
static uint16_t Tilt_ShakeWord(void)
{
    return (uint16_t)((uint32_t)g_ShakeY >> 16);
}

/* Stage2_ArenaDrawTilt - draws the arena's tilting floor with scroll alone: plane A's line
 * H-scroll and column V-scroll ramps from the slope about the pivot x and the floor y, and plane
 * B's ramps above and below the split line it computes for the V-int (the hull picture above,
 * plane A's name table below)
 * In:  g_Stage2Vars->slope; g_Stage2Vars2 pivot, floor, picture position; g_ShakeY;
 *      g_Stage2BossPlaneOn / g_Stage2BossPlaneAlt
 * Out: g_HScrollLinesA/B, col_a / col_b / col_b_lower ramps; split_next (0: no picture, $80: the
 *      hull out of view, else the line); registers as the last ramp (Mem_FillRamp) leaves them
 * Evidence: Mem_FillRamp with d1 = -slope (plane A: 224 lines from -(floor + shake) * -slope - 8
 *           px, columns from the long at $F5AC + (shake - pivot * -slope)); split = min(-slope*256 +
 *           d2, d2) negated + picture y + $86 (>= $40); Mem_FillLongs of 0 and $800080 when the hull
 *           is out of view ($9E0000 / $850000); the lower ramp from $FD500000 + line * slope; the
 *           tail of Stage2_UpdateScroll in arena mode (TAS f32066-f35416, 3351 frames). The
 *           prototype makes d0/d7/a0/a1 outputs (measured clobbers) so the final jmp
 *           Mem_FillRamp gets them
 */
void Stage2_ArenaDrawTilt(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                          uint32_t *d7, uint32_t *a0, uint32_t *a1)
{
    Stage2Vars *v = g_Stage2Vars;
    Stage2Vars2 *v2 = g_Stage2Vars2;
    uint16_t pivot = (uint16_t)((uint32_t)v2->pivot_x >> 16);   /* the long's integer word */
    uint16_t floor = (uint16_t)((uint32_t)v2->floor_y >> 16);

    /* plane A: the line H-scroll ramp, then the column V-scroll ramp */
    *d1 = (uint32_t)(int32_t)(int16_t)(uint16_t)(0u - (uint16_t)v->slope);   /* neg.w / ext.l */
    *d2 = (0u - Tilt_Muls(pivot, *d1)) + (uint32_t)g_ShakeY;
    *d0 = 0u - Tilt_Muls((uint16_t)(floor + Tilt_ShakeWord()), *d1);
    *d3 = *d0;
    *d4 = *d1;
    uint32_t count = Tilt_Low(*d7, ARENA_LINES);
    *d0 -= ARENA_LINE_BIAS;
    *a0 = dk::addr(g_HScrollLinesA);
    Mem_FillRamp(d0, *d1, d2, *d3, (uint16_t)count, a0, a1);
    *d0 = ((((uint32_t)v2->floor_y) << 16) | v2->roll_lo) + *d2;   /* move.l $F5AC: the long at +$E */
    *d1 <<= 4;
    *d5 = *d1;
    count = Tilt_Low(count, ARENA_COLUMNS);
    *a0 = dk::addr(v->col_a);
    Mem_FillRamp(d0, *d1, d2, *d3, (uint16_t)count, a0, a1);

    if (!g_Stage2BossPlaneOn) {
        count = Tilt_Low(count, ARENA_NO_PICTURE_HSCROLL);
        v2->split_next = 0;
    } else {
        /* the split line: where the floor's top edge crosses, clamped */
        count = (*d5 << 4) + *d2;
        if ((int32_t)count > (int32_t)*d2)
            count = *d2;
        count = Tilt_Swap(count);
        count = Tilt_Low(count, (uint16_t)(0u - (uint16_t)count));
        count = Tilt_Low(count, (uint16_t)((uint16_t)count + (uint16_t)((uint32_t)v2->picture_y >> 16)
                                           + ARENA_SPLIT_BIAS));
        if ((int16_t)(uint16_t)count < ARENA_SPLIT_MIN)
            count = Tilt_Low(count, ARENA_SPLIT_MIN);
        v2->split_next = (uint16_t)count;
        *d2 -= (uint32_t)v2->picture_y;
        int32_t limit = v->slope >= 0 ? ARENA_SHOW_PICTURE_UP : ARENA_SHOW_PICTURE_DOWN;
        if ((int32_t)*d2 > limit) {
            /* the hull out of view: plane B blank above, grey columns */
            v2->split_next = ARENA_SPLIT_NONE_LINE;
            *d0 = 0;
            *a0 = dk::addr(g_HScrollLinesB);
            uint16_t n = 0x40;
            Mem_FillLongs(*d0, *d1, d2, *d3, &n, a0, a1);
            *d0 = ARENA_GREY_COLUMNS;
            *a0 = dk::addr(v->col_b);
            n = 0x0A;
            Mem_FillLongs(*d0, *d1, d2, *d3, &n, a0, a1);
        } else {
            /* the hull picture above the split: plane B's ramps */
            *d0 = (uint32_t)v2->picture_x + *d3;
            *d1 = *d4;
            if (g_Stage2BossPlaneAlt)
                *d0 += ARENA_ALT_HALF;
            *a0 = dk::addr(g_HScrollLinesB);
            Mem_FillRamp(d0, *d1, d2, *d3, (uint16_t)count, a0, a1);
            *d0 = *d2;
            *d1 = *d5;
            *a0 = dk::addr(v->col_b);
            count = Tilt_Low(count, ARENA_COLUMNS);
            Mem_FillRamp(d0, *d1, d2, *d3, (uint16_t)count, a0, a1);
        }
        count = 0;   /* moveq #0,d7: the split line's H-scroll word */
    }

    /* plane B below the split: plane A's floor again, from the split line down */
    *d3 = Tilt_Low(*d3, v2->split_next);
    *d1 = (uint32_t)(int32_t)v->slope;
    *d2 = (0u - Tilt_Muls(pivot, *d1)) + (uint32_t)g_ShakeY;
    *d0 = (0u - Tilt_Muls((uint16_t)(floor + Tilt_ShakeWord()), *d1)) + ARENA_LOWER_BIAS;
    *d4 = Tilt_Muls(*d3, *d1);
    *d0 += *d4;
    u16 *line = (u16 *)g_HScrollLinesB + (int16_t)(uint16_t)*d3;   /* adda.w d3 twice */
    *line++ = (uint16_t)count;
    *a0 = dk::addr(line);
    count = Tilt_Low(count, (uint16_t)(0xDF - (uint16_t)*d3));
    Mem_FillRamp(d0, *d1, d2, *d3, (uint16_t)count, a0, a1);
    *d0 = *d2;
    *d1 <<= 4;
    *d7 = Tilt_Low(count, ARENA_COLUMNS);
    *a0 = dk::addr(v->col_b_lower);
    dk::jump(at_Mem_FillRamp);   /* jmp: its rts returns to our caller */
}

/* the tilted floor's screen y under x: $C4 - ((x - pivot) * slope) >> 16, in d0's low word (the
   product's low word above it: swap / neg.w / addi.w) */
static uint32_t Tilt_FloorY(uint16_t x)
{
    uint16_t pivot = (uint16_t)((uint32_t)g_Stage2Vars2->pivot_x >> 16);
    uint32_t p = Tilt_Swap(Tilt_Muls((uint16_t)(x - pivot), (uint16_t)g_Stage2Vars->slope));
    return Tilt_Low(p, (uint16_t)(ARENA_FLOOR_SURFACE_Y - (uint16_t)p));
}

/* Coll_Stage2TiltFloor - terrain routine of the stage 2 arena: with floor mask bit 0, lands the
 * mover when its new y reaches the tilted floor's y under it
 * In:  dx/dy = the move (d1/d2 words), x/y = the position (d4/d5), mask = d7 (bit 0: the floor)
 * Out: x += dx; landed: y = the floor y, d7.w = 1 (upper word kept); else y += dy, d7 = 0;
 *      d3 = d6 = 0; d0 = the floor y (with the mask bit; else unchanged)
 * Evidence: moveq #0,d3 / moveq #0,d6 / add.w d1,d4 / btst #0,d7 / sub.w $F5A6 / muls.w
 *           g_SceneVars+6 / swap / neg.w / addi.w #$C4 / add.w d2,d5 / cmp.w d0,d5 / blt / move.w
 *           d0,d5 / move.w #1,d7; installed as g_TerrainFn by Stage2_LiftArriveTop
 */
void Coll_Stage2TiltFloor(uint32_t *d0, uint16_t dx, uint16_t dy, uint32_t *d3, uint16_t *x, uint16_t *y, uint32_t *d6,
                          uint32_t *mask)
{
    *d3 = 0;
    *d6 = 0;
    *x = (uint16_t)(*x + dx);
    *y = (uint16_t)(*y + dy);
    if (*mask & 1) {
        *d0 = Tilt_FloorY(*x);
        if ((int16_t)*y >= (int16_t)(uint16_t)*d0) {   /* blt keeps the move */
            *y = (uint16_t)*d0;
            *mask = Tilt_Low(*mask, 1);
            return;
        }
    }
    *mask = 0;
}

/* Coll_Stage2TiltFloorRest - resting variant of Coll_Stage2TiltFloor: with mask bit 0 keeps the
 * mover on the tilted floor wherever it moves
 * In:  as Coll_Stage2TiltFloor
 * Out: x += dx; mask bit 0: y = the floor y, d7.w = 1 (upper word kept), d0 = the floor y; else
 *      y += dy, d7 = 0; d3 = d6 = 0
 * Evidence: as Coll_Stage2TiltFloor without the cmp (move.w d0,d5 at once); installed as
 *           g_TerrainFnAlt by Stage2_LiftArriveTop (grounded objects)
 */
void Coll_Stage2TiltFloorRest(uint32_t *d0, uint16_t dx, uint16_t dy, uint32_t *d3, uint16_t *x, uint16_t *y,
                              uint32_t *d6, uint32_t *mask)
{
    *d3 = 0;
    *d6 = 0;
    *x = (uint16_t)(*x + dx);
    if (*mask & 1) {
        *d0 = Tilt_FloorY(*x);
        *y = (uint16_t)*d0;
        *mask = Tilt_Low(*mask, 1);
        return;
    }
    *y = (uint16_t)(*y + dy);
    *mask = 0;
}

/* Flight_UpdateCorridor - per-frame scroll inputs of the flying section's corridor: the vertical
 * position from the camera, the floor/ceiling stripe phase from g_MapScrollX mod $A000, the 32
 * middle-wall band offsets advanced by their rates (Tbl_024880); then draws it
 * In:  g_CameraY, g_MapScrollX
 * Out: g_FlightCorridor vpos / phase / band[] updated; d7.w = $FFFF; then Flight_DrawCorridor
 * Evidence: move.l g_CameraY,d0 / + d0 lsr 2 / addi.l #$E80000 / neg.l / andi.l #$FFFFFF / divu.w
 *           #$A000 of g_MapScrollX + $A000 / clr.w / lsr.l #8 / neg.l; d1 = -(g_MapScrollX << 6) -
 *           band[0]; band[i] += rate[i] + d1 (32, dbra); bra Flight_DrawCorridor; called by $33352
 *           (jmp) and $33448 in the flying scene $33274
 */
void Flight_UpdateCorridor(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint16_t *d6, uint32_t *d7,
                           uint32_t *a4)
{
    FlightCorridorVars *v = g_FlightCorridor;
    uint32_t cam = (uint32_t)g_CameraY;
    v->vpos = (int32_t)((0u - (cam + (cam >> 2) + CORRIDOR_Y_BIAS)) & 0xFFFFFFu);

    /* divu.w: the quotient must fit a word, else the dividend stays (and so the "remainder") */
    uint32_t x = (uint32_t)g_MapScrollX + CORRIDOR_STRIPE;
    uint32_t q = x / CORRIDOR_STRIPE;
    if (q <= 0xFFFFu)
        x = ((x % CORRIDOR_STRIPE) << 16) | q;
    x &= 0xFFFF0000u;                                     /* clr.w: the remainder */
    v->phase = (int32_t)(0u - (x >> 8));

    PTR(const u32) rate;
    rate.v = Tbl_024880;
    *d1 = (0u - ((uint32_t)g_MapScrollX << 6)) - (uint32_t)v->band[0];
    for (int i = 0; i < CORRIDOR_BANDS; i++)
        v->band[i] = (int32_t)((uint32_t)v->band[i] + (uint32_t)*rate++ + *d1);
    *d7 = (*d7 & 0xFFFF0000u) | 0xFFFFu;                  /* dbra ends at -1 */
    dk::jump(at_Flight_DrawCorridor);                     /* bra into the next file */
}
