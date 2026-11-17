#include "game.h"

/* turret 1's a2 script (ROM data right after its spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_026148 = 0x026148u;

/* swap: the 68000's halves exchanged */
static uint32_t TiltPoint_Swap(uint32_t v)
{
    return (v << 16) | (v >> 16);
}

/* muls.w: the low words, signed, multiplied into a long */
static uint32_t TiltPoint_Muls(uint32_t a, uint32_t b)
{
    return (uint32_t)((int32_t)(int16_t)a * (int32_t)(int16_t)b);
}

/* Boss_Stage2_TiltPoint - maps a point of the hull picture to the screen with the arena tilt:
 * the picture position plus the point, sheared by the floor slope about the pivot x (for y) and
 * the floor y (for x)
 * In:  px/py = the point's offset in the hull picture (d0/d1); g_Stage2Vars->slope,
 *      g_Stage2Vars2 pivot_x, floor_y, picture_x/y (16.16)
 * Out: d4.w/d5.w = screen x/y, their fractions in the high words; d2.w = the slope, d3 the last
 *      shear term (scratch)
 * Evidence: y = picture y + ((picture x - pivot) + px) * slope + py, x = picture x - ((picture y
 *           - floor y) + py) * slope + px (muls.w by g_SceneVars+6), with a slope*16 bias in x
 *           and -slope*32 in y when the slope is negative; callers Boss_Stage2_PlaceTurret,
 *           Boss_Stage2_FxAtHullPoint; TAS f32140-f35416 (3277 frames)
 */
void Boss_Stage2_TiltPoint(uint16_t px, uint16_t py, uint16_t *slope_out, uint32_t *d3, uint32_t *d4, uint32_t *d5)
{
    const Stage2Vars2 *v2 = g_Stage2Vars2;
    uint16_t slope = (uint16_t)g_Stage2Vars->slope;
    *slope_out = slope;

    /* the starting terms: x = slope * 16; y = 0, or -slope * 32 when the slope is negative */
    uint32_t x = (uint32_t)((int32_t)(int16_t)slope << 4);   /* ext.l / asl.l #4 */
    uint32_t y = 0;                                           /* moveq #0,d5 */
    if ((int32_t)x < 0)                                       /* bpl skips */
        y = (uint32_t)-((int32_t)(int16_t)slope << 5);       /* ext.l / asl.l #5 / neg.l */

    /* y: the x distance from the pivot (integer part) plus px, times the slope */
    uint32_t t = TiltPoint_Swap((uint32_t)v2->picture_x - (uint32_t)v2->pivot_x);
    t = (t & 0xFFFF0000u) | (uint16_t)(t + px);
    t = TiltPoint_Muls(t, slope) + (uint32_t)v2->picture_y;
    y = TiltPoint_Swap(y + t);
    y = (y & 0xFFFF0000u) | (uint16_t)(y + py);

    /* x: the y distance from the floor (integer part) plus py, times minus the slope */
    t = TiltPoint_Swap((uint32_t)v2->picture_y - (uint32_t)v2->floor_y);
    t = (t & 0xFFFF0000u) | (uint16_t)(t + py);
    t = (uint32_t)-(int32_t)TiltPoint_Muls(t, slope) + (uint32_t)v2->picture_x;
    *d3 = t;
    x = TiltPoint_Swap(x + t);
    x = (x & 0xFFFF0000u) | (uint16_t)(x + px);

    *d4 = x;
    *d5 = y;
}

/* Boss_Stage2_SpawnTurret1 - spawns turret 1 of the stage 2 arena boss (Scr2_026148: bit 1, hull
 * offset ($B7, $63), the leftmost), an unlinked boss-class object the boss runs inline
 * (Boss_Stage2_MoveHullRunTurrets)
 * In:  x/y = where it starts (Op2_SpawnChildOrWait passes the boss's spawn position)
 * Out: script = Scr2_026148, then Obj_SpawnBossUnlinked: Z set and a6 = the turret, Z clear when
 *      the pool is short
 * Evidence: lea Scr2_026148,a2 / jmp Obj_SpawnBossUnlinked; dc.l at $025C0E (field $1E); Scr2_026148
 *           sets vars 0/1/2 = 1, $B7, $63 then the shared turret body $0261AC; RAM f32220: x 98.
 *           The prototype gains &a2 (measured a clobber) so the jump hands the script on
 */
void Boss_Stage2_SpawnTurret1(uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t a4, uint32_t *a6)
{
    *script = Scr2_026148;
    dk::jump(at_Obj_SpawnBossUnlinked);   /* jmp: it returns to our caller */
}
