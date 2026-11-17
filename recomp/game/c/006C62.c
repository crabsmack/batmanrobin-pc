#include "game.h"

/* Op5_SetLoopA0-A8 / Op5_SetLoopA ($006C62-$006C8B): loop counter A of the wave director, set to
 * a constant (moveq) or to a word; Op5_LoopA counts it down. */

/* the shared store ($6C84): loop counter A = d0's low word, then NEXT */
static void store_loop_a(uint32_t d0, const u16 **script, Vm5Wave *self)
{
    self->loop_a = (uint16_t)d0;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op5_SetLoopA0 - a5 op: sets loop counter A to 0 (an Op5_LoopA then falls through at once).
 * In:  script (no operands); a6 = the wave object
 * Out: d0 = 0; self->loop_a; NEXT
 * Evidence: moveq #0,d0 / bra.s $6C84 (move.w d0,$1A(a6)); script uses $21E08, $21E18; 19 corpus runs
 */
void Op5_SetLoopA0(uint32_t *d0, const u16 **script, Vm5Wave *self)
{
    *d0 = 0;
    store_loop_a(*d0, script, self);
}

/* Op5_SetLoopA2 - a5 op: sets loop counter A to 2 (the body before Op5_LoopA runs twice).
 * In:  as Op5_SetLoopA0
 * Out: d0 = 2; self->loop_a; NEXT
 * Evidence: moveq #2,d0 / bra.s $6C84; 89 script uses, e.g. $1754C (`6C66 6DD4`); 16 corpus runs
 */
void Op5_SetLoopA2(uint32_t *d0, const u16 **script, Vm5Wave *self)
{
    *d0 = 2;
    store_loop_a(*d0, script, self);
}

/* Op5_SetLoopA3 - a5 op: sets loop counter A to 3.
 * In:  as Op5_SetLoopA0
 * Out: d0 = 3; self->loop_a; NEXT
 * Evidence: moveq #3,d0 / bra.s $6C84; script uses $21E5E, $22CBA; 5 corpus runs
 */
void Op5_SetLoopA3(uint32_t *d0, const u16 **script, Vm5Wave *self)
{
    *d0 = 3;
    store_loop_a(*d0, script, self);
}

/* Op5_SetLoopA4 - a5 op: sets loop counter A to 4.
 * In:  as Op5_SetLoopA0
 * Out: d0 = 4; self->loop_a; NEXT
 * Evidence: moveq #4,d0 / bra.s $6C84; script uses $21FDA, $2220E; 9 corpus runs
 */
void Op5_SetLoopA4(uint32_t *d0, const u16 **script, Vm5Wave *self)
{
    *d0 = 4;
    store_loop_a(*d0, script, self);
}

/* Op5_SetLoopA5 - a5 op: sets loop counter A to 5.
 * In:  as Op5_SetLoopA0
 * Out: d0 = 5; self->loop_a; NEXT
 * Evidence: moveq #5,d0 / bra.s $6C84; script uses $21EA6, $21F8A; 9 corpus runs
 */
void Op5_SetLoopA5(uint32_t *d0, const u16 **script, Vm5Wave *self)
{
    *d0 = 5;
    store_loop_a(*d0, script, self);
}

/* Op5_SetLoopA6 - a5 op: sets loop counter A to 6.
 * In:  as Op5_SetLoopA0
 * Out: d0 = 6; self->loop_a; NEXT
 * Evidence: moveq #6,d0 / bra.s $6C84; script uses $23DC0, $23E9C; 15 corpus runs
 */
void Op5_SetLoopA6(uint32_t *d0, const u16 **script, Vm5Wave *self)
{
    *d0 = 6;
    store_loop_a(*d0, script, self);
}

/* Op5_SetLoopA7 - a5 op: sets loop counter A to 7.
 * In:  as Op5_SetLoopA0
 * Out: d0 = 7; self->loop_a; NEXT
 * Evidence: moveq #7,d0 / bra.s $6C84; script uses $3C4AC, $3C4CC; 3 corpus runs
 */
void Op5_SetLoopA7(uint32_t *d0, const u16 **script, Vm5Wave *self)
{
    *d0 = 7;
    store_loop_a(*d0, script, self);
}

/* Op5_SetLoopA8 - a5 op: sets loop counter A to 8.
 * In:  as Op5_SetLoopA0
 * Out: d0 = 8; self->loop_a; NEXT
 * Evidence: moveq #8,d0 / bra.s $6C84; script uses $3C5B8, $3C5D0; 3 corpus runs
 */
void Op5_SetLoopA8(uint32_t *d0, const u16 **script, Vm5Wave *self)
{
    *d0 = 8;
    store_loop_a(*d0, script, self);
}

/* Op5_SetLoopA - a5 op: sets loop counter A to a word.
 * In:  script = word count; a6 = the wave object
 * Out: d0's low word = the count; self->loop_a; NEXT
 * Evidence: move.w (a5)+,d0 / move.w d0,$1A(a6) / NEXT; script uses $21762 (`6C82 000C`), $23D02; 8
 *           corpus runs
 */
void Op5_SetLoopA(uint16_t *d0, const u16 **script, Vm5Wave *self)
{
    *d0 = *(*script)++;
    store_loop_a(*d0, script, self);
}
