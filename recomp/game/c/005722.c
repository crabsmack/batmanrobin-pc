#include "game.h"

/* Op4_JitterSpawnHeading / Op4_JitterDrawXY ($005722-$005749): random offsets for the spawn
 * heading register and the drawn point. */

/* NEXT: the handler the script's next word names, jumped to */
static void op_next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Rand_Next's signed low word times the operand w (muls.w (a4)+,d0): [-w/2, w/2) in the high word */
static uint32_t jitter(const u16 **script)
{
    int16_t r = (int16_t)(uint16_t)(uint32_t)Rand_Next();
    return (uint32_t)((int32_t)r * (int16_t)(uint16_t)*(*script)++);
}

/* Op4_JitterSpawnHeading - a4 op: turns the spawn heading register d3 by a random [-w/2, w/2)
 * heading units ($400 a turn).
 * In:  script = word w; d3 = spawn heading (16.16)
 * Out: d0 = the offset; d3 += it; NEXT
 * Evidence: bsr Rand_Next / muls.w (a4)+,d0 / add.l d0,d3; script uses $1A45A, $1A54E (`5722 0400`:
 *           any direction, then Op4_SpawnAddVel), $14BCE; 66 corpus runs
 */
void Op4_JitterSpawnHeading(uint32_t *d0, uint32_t *d3, const u16 **script)
{
    *d0 = jitter(script);
    *d3 += *d0;
    uint32_t handler;
    op_next(&handler, script);
}

/* Op4_JitterDrawXY - a4 op: shakes the drawn point: random [-wx/2, wx/2) / [-wy/2, wy/2) px added
 * to the drawn x/y words in d4/d5 (the swap puts the product's integer word in the low word, its
 * fraction in the high word, which lands on d4/d5's upper word too).
 * In:  script = word wx, word wy; d4/d5 = drawn x/y (low words, after the motion op's swap)
 * Out: d0 = the y offset, swapped; d4/d5 += the swapped offsets; NEXT
 * Evidence: bsr Rand_Next / muls.w (a4)+,d0 / swap d0 / add.l d0,d4, the same for d5; uses $1A5C6,
 *           $1F2BC between the motion op $6286 and Op4_SetResume + the draw op $67D2; 1 corpus run
 */
void Op4_JitterDrawXY(uint32_t *d0, uint32_t *d4, uint32_t *d5, const u16 **script)
{
    uint32_t dx = jitter(script);
    *d4 += dx << 16 | dx >> 16;
    uint32_t dy = jitter(script);
    *d0 = dy << 16 | dy >> 16;
    *d5 += *d0;
    uint32_t handler;
    op_next(&handler, script);
}
