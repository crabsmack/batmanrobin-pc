#include "game.h"

/* Op4_ChildJitterHeading / ChildScatterHeading / ChildJitterVelX / ChildScatterVelX
 * ($005D74-$005DCB): random turns of the last spawned child (a5) and random kicks to its x
 * velocity. Jitter = +-w/2, Scatter = +-(base + [0, w/2)): the jitter's sign picks the
 * direction of the base. */

/* add.l d0,<long>: the sum, and the add's condition codes for the caller (X = C = the carry) */
static uint32_t add_long_ccr(uint32_t old, uint32_t d, dk::Flags *ccr)
{
    uint32_t sum = old + d;
    bool carry = sum < old;
    *ccr = dk::Flags{ carry, (sum >> 31) != 0, sum == 0, ((~(old ^ d)) & (old ^ sum) & 0x80000000u) != 0, carry };
    return sum;
}

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Rand_Next's low word, signed */
static int16_t rand_word(void) { return (int16_t)(uint16_t)(uint32_t)Rand_Next(); }

/* the 8.8 jitter: Rand_Next's low word asr #4 ([-$800, $800)) times w (muls.w), asr.l #4:
 * [-w/2, w/2) in 8.8, as 16.16 */
static uint32_t jitter_8_8(int16_t r, const u16 **script)
{
    return (uint32_t)(((int32_t)(int16_t)(r >> 4) * (int16_t)(uint16_t)*(*script)++) >> 4);
}

/* an 8.8 operand word as 16.16: ext.l / asl.l #8 */
static uint32_t word_8_8(const u16 **script)
{
    return (uint32_t)(int32_t)(int16_t)(uint16_t)*(*script)++ << 8;
}

/* Op4_ChildJitterHeading - a4 op: turns the child's heading by a random [-w/2, w/2) units ($400 a
 * turn): Rand_Next's signed low word times w, added to the heading long.
 * In:  script = word w; child = a5
 * Out: d0 = the turn (16.16); child->heading; the add.l's condition codes; NEXT
 * Evidence: bsr Rand_Next / muls.w (a4)+,d0 / add.l d0,$2A(a5) / NEXT; 61 a4-script uses, e.g.
 *           $25B72, $25B90 (spawn, random direction, then Op4_ChildAddVelRadius); 150 corpus runs
 */
void Op4_ChildJitterHeading(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr)
{
    int16_t r = rand_word();
    *d0 = (uint32_t)((int32_t)r * (int16_t)(uint16_t)*(*script)++);
    vm4_set_fix(child->heading, child->heading_frac,
                add_long_ccr(vm4_fix(child->heading, child->heading_frac), *d0, ccr));
    next(handler, script);
}

/* Op4_ChildScatterHeading - a4 op: turns the child's heading by base + a random [0, w/2) units,
 * clockwise or anticlockwise at random (the sign of Rand_Next * w).
 * In:  script = word base (whole units, unsigned), word w; child = a5
 * Out: child->heading; NEXT
 * Evidence: bsr Rand_Next / moveq #0,d1 / move.w (a4)+,d1 / swap d1 / muls.w (a4)+,d0 / bpl /
 *           neg.l d1 / add.l d1,d0 / add.l d0,$2A(a5); script use $26AD2; 3 corpus runs (fuzz only)
 */
void Op4_ChildScatterHeading(const u16 **script, Vm4Obj *child)
{
    int16_t r = rand_word();
    uint32_t base = (uint32_t)(uint16_t)*(*script)++ << 16;
    int32_t part = (int32_t)r * (int16_t)(uint16_t)*(*script)++;
    if (part < 0)
        base = 0u - base;
    vm4_add_fix(child->heading, child->heading_frac, (uint32_t)part + base);
    uint32_t handler;
    next(&handler, script);
}

/* Op4_ChildJitterVelX - a4 op: adds a random [-w/2, w/2) (8.8 px/frame) to the child's x velocity.
 * In:  script = word w (8.8); child = a5
 * Out: d0 = the amount (16.16); child->vel_x; the add.l's condition codes; NEXT
 * Evidence: bsr Rand_Next / asr.w #4,d0 / muls.w (a4)+,d0 / asr.l #4,d0 / add.l d0,$3E(a5); script
 *           uses $1A638, $1A69E, $1A6CE; 17 corpus runs
 */
void Op4_ChildJitterVelX(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr)
{
    *d0 = jitter_8_8(rand_word(), script);
    child->vel_x = (int32_t)add_long_ccr((uint32_t)(int32_t)child->vel_x, *d0, ccr);
    next(handler, script);
}

/* Op4_ChildScatterVelX - a4 op: kicks the child's x velocity by base + a random [0, w/2) (8.8),
 * left or right at random.
 * In:  script = word base (8.8), word w (8.8); child = a5
 * Out: d1 = +-base (16.16); d0 = the kick; child->vel_x; the add.l's condition codes; NEXT
 * Evidence: the Op4_ScatterVelX code on $3E(a5); 23 a4-script uses, e.g. $1A3B0 (`5DAE 0100 FF00`),
 *           $1A650; 22 corpus runs
 */
void Op4_ChildScatterVelX(uint32_t *d0, uint32_t *d1, uint32_t *handler, const u16 **script, Vm4Obj *child,
                          dk::Flags *ccr)
{
    int16_t r = rand_word();
    *d1 = word_8_8(script);
    *d0 = jitter_8_8(r, script);
    if ((int32_t)*d0 < 0)
        *d1 = 0u - *d1;
    *d0 += *d1;
    child->vel_x = (int32_t)add_long_ccr((uint32_t)(int32_t)child->vel_x, *d0, ccr);
    next(handler, script);
}
