#include "game.h"

/* The effect-spawn heading ops ($005306-$005377): set the heading register d3 (heading word in the
 * high word, $400 a turn) for a later Op2_FxSpawnRegs*. */

/* the next handler word (NEXT) */
static uint32_t next_op(const u16 **script)
{
    return dk::code((int16_t)(uint16_t)*(*script)++);
}

/* sprite attribute bit 11: the sprite is mirrored (facing left) */
static constexpr uint16_t FX_ATTR_FLIP_H = 0x0800;

/* `divu.w n,d0` of $40000 (the size of one of n directions, in heading units << 8): quotient low,
   remainder high; a quotient over $FFFF (n <= 4) sets V and leaves d0 as it was. The scripts'
   n are never 0 (that would be the divide-by-zero trap). */
static uint32_t direction_size(uint16_t n)
{
    uint32_t q = 0x40000u / n, r = 0x40000u % n;
    return q > 0xFFFFu ? 0x40000u : (r << 16 | q);
}

/* the motion heading's direction index among n: (n * heading + $200) >> 10, as the asm computes
   it on d3 (mulu.w / addi.l #$200 / lsr.l #8 / lsr.w #2: the upper word is the lsr.l's) */
static uint32_t direction_index(uint16_t n, const Vm2Motion *mot)
{
    uint16_t heading = (uint16_t)((uint32_t)(int32_t)mot->heading >> 16);
    uint32_t v = ((uint32_t)n * heading + 0x200u) >> 8;
    return (v & 0xFFFF0000u) | (uint16_t)((uint16_t)v >> 2);
}

/* Op2_FxHeading - a2 op: sets the spawn heading to a word operand (0-$3FF).
 * In:  script = word heading
 * Out: d3 = heading : d3's old high word (move.w then swap); NEXT
 * Evidence: move.w (a2)+,d3 / swap d3; 16 corpus runs; $1C2CA `5306 0300 5198`
 */
void Op2_FxHeading(uint32_t *d3, const u16 **script)
{
    uint16_t heading = *(*script)++;
    *d3 = (uint32_t)heading << 16 | *d3 >> 16;
    dk::jump(next_op(script));
}

/* Op2_FxHeadingByFacing - a2 op: sets the spawn heading from a pair by the sprite's facing: the
 * first word when it is mirrored (facing left), the second otherwise.
 * In:  script = word heading facing left, word heading facing right; spr (attr)
 * Out: d3 = the heading in the high word, low word 0; NEXT
 * Evidence: move.l (a2)+,d3 / btst #3,$38(a5) / bne / swap d3 / clr.w d3; 18 corpus runs; $258FC
 *           `530E 0180 0080` ($180 down-left / $80 down-right)
 */
void Op2_FxHeadingByFacing(uint32_t *d3, const u16 **script, const Vm2Sprite *spr)
{
    uint16_t left = (*script)[0], right = (*script)[1];
    *script += 2;
    *d3 = (uint32_t)((spr->attr & FX_ATTR_FLIP_H) ? left : right) << 16;
    dk::jump(next_op(script));
}

/* Op2_FxHeadingSnap - a2 op: sets the spawn heading to the object's heading rounded to the nearest
 * of n directions.
 * In:  script = word n; mot (heading)
 * Out: d0 = $40000 divu n; d3 = index * the quotient << 8 (heading in the high word); NEXT
 * Evidence: move.w (a2)+,d3 / move.l #$40000,d0 / divu.w d3,d0 / mulu.w $C(a3),d3 / addi.l #$200,d3 /
 *           lsr.l #8,d3 / lsr.w #2,d3 / mulu.w d0,d3 / lsl.l #8,d3; 14 corpus runs; $2FB20 `5320 0010 5198`
 */
void Op2_FxHeadingSnap(uint32_t *d0, uint32_t *d3, const u16 **script, const Vm2Motion *mot)
{
    uint16_t n = *(*script)++;
    *d0 = direction_size(n);
    *d3 = direction_index(n, mot);
    *d3 = ((uint32_t)(uint16_t)*d3 * (uint16_t)*d0) << 8;
    dk::jump(next_op(script));
}

/* Op2_FxHeadingSnapClamp - a2 op: as Op2_FxHeadingSnap with the direction index clamped to
 * [lo, hi] (signed word compares).
 * In:  script = word n, word lo, word hi; mot (heading)
 * Out: as Op2_FxHeadingSnap; NEXT
 * Evidence: the Op2_FxHeadingSnap code, then cmp.w (a2)+,d3 / bge / move.w -2(a2),d3 twice (ble for
 *           hi), then mulu.w d0,d3 / lsl.l #8; 6 corpus runs; $26344 `5340 0028 0005 000F`
 */
void Op2_FxHeadingSnapClamp(uint32_t *d0, uint32_t *d3, const u16 **script, const Vm2Motion *mot)
{
    uint16_t n = *(*script)++;
    *d0 = direction_size(n);
    *d3 = direction_index(n, mot);
    uint16_t lo = *(*script)++;
    if ((int16_t)(uint16_t)*d3 < (int16_t)lo)
        *d3 = (*d3 & 0xFFFF0000u) | lo;
    uint16_t hi = *(*script)++;
    if ((int16_t)(uint16_t)*d3 > (int16_t)hi)
        *d3 = (*d3 & 0xFFFF0000u) | hi;
    *d3 = ((uint32_t)(uint16_t)*d3 * (uint16_t)*d0) << 8;
    dk::jump(next_op(script));
}
