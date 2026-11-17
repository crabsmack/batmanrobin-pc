#include "game.h"

static uint32_t swap32(uint32_t v) { return (v << 16) | (v >> 16); }
static uint32_t set_low(uint32_t r, uint16_t v) { return (r & 0xFFFF0000u) | v; }

/* Boss_CatHead_SwayPoint - maps a point of the cat-head's face through the arena's sway, the shear
 * g_SceneVars+$10 about the pivot longs at +$12 and +$14 (the ramp Boss_CatHead_SwayRaster draws): the
 * x moves by the shear times the point's y offset from the pivot line, the y by the shear times its x
 * offset from the pivot column (with an extra -32 x shear lift when the shear is negative)
 * In:  d0/d1 (low words) = the point's x/y; the arena's shear, pivot_x/pivot_y and the word after them
 * Out: d2's low word = the shear; d3 = -(the y term), the product of muls.w; d4/d5 = the swayed x/y as
 *      16.16 with the integer in the LOW word (the callers swap them)
 * Evidence: moveq #0,d5 / move.w (g_SceneVars+$10).w,d2 / ext.l / asl.l #3 / bpl / asl.l #5 / neg.l /
 *           move.l (g_SceneVars+$12).w,d3 / neg.l / swap / add.w d0 / muls.w d2 / add.l / swap / add.w d1;
 *           move.l (g_SceneVars+$14).w,d3 / neg.l / swap / add.w d1 / muls.w d2 / neg.l / add.l / swap /
 *           add.w d0 / rts; the bsr.b at $03B7D2 of the eye natives (Boss_CatHead_EyeSparkLeft ...); no
 *           corpus run reaches it (raw-stubs 2026-10-30); 03B760.syn checks it. The longs read the pivot
 *           words in pairs as the asm does (+$12/+$14, then +$14/+$16)
 */
void Boss_CatHead_SwayPoint(uint32_t d0, uint32_t d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5)
{
    const CatHeadArena *arena = cat_head_arena();
    uint16_t shear = (uint16_t)arena->shear;
    *d2 = set_low(*d2, shear);
    uint32_t s = (uint32_t)(int32_t)(int16_t)shear;                     /* ext.l */
    uint32_t x = s << 3;                                                 /* asl.l #3 */
    uint32_t y = 0;                                                      /* moveq #0,d5 */
    if ((int32_t)x < 0)
        y = 0u - (s << 5);                                               /* ext.l / asl.l #5 / neg.l */
    uint32_t p = ((uint32_t)(uint16_t)arena->pivot_x << 16) | (uint16_t)arena->pivot_y;
    p = swap32(0u - p);
    p = set_low(p, (uint16_t)((uint16_t)p + (uint16_t)d0));              /* add.w d0,d3 */
    uint32_t m = (uint32_t)((int32_t)(int16_t)(uint16_t)p * (int32_t)(int16_t)shear);   /* muls.w */
    y = swap32(y + m);
    y = set_low(y, (uint16_t)((uint16_t)y + (uint16_t)d1));              /* add.w d1,d5 */
    uint32_t q = ((uint32_t)(uint16_t)arena->pivot_y << 16) | (uint16_t)arena->unk_16;
    q = swap32(0u - q);
    q = set_low(q, (uint16_t)((uint16_t)q + (uint16_t)d1));              /* add.w d1,d3 */
    uint32_t m2 = 0u - (uint32_t)((int32_t)(int16_t)(uint16_t)q * (int32_t)(int16_t)shear);  /* muls / neg.l */
    x = swap32(x + m2);
    x = set_low(x, (uint16_t)((uint16_t)x + (uint16_t)d0));              /* add.w d0,d4 */
    *d3 = m2;
    *d4 = x;
    *d5 = y;
}
