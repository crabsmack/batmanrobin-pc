#include "game.h"

/* Op5_SetLoopB and its store ($006CE2-$006CEB): loop counter B of the wave director; the
 * constant variants (Op5_SetLoopB2 / B3 / B5) branch to the store with d0 set. */

/* Op5_SetLoopB - a5 op: sets loop counter B to a word.
 * In:  script = word count; a6 = the wave object
 * Out: d0's low word = the count; then as Op5_SetLoopB_Store
 * Evidence: move.w (a5)+,d0, falling into Op5_SetLoopB_Store; script use $343AE (`6CE2 000A`); 3
 *           corpus runs
 */
void Op5_SetLoopB(uint16_t *d0, uint32_t *handler, const u16 **script, Vm5Wave *self)
{
    *d0 = *(*script)++;
    Op5_SetLoopB_Store(*d0, handler, script, self);
}

/* Op5_SetLoopB_Store - the shared store of the Op5_SetLoopB* ops: loop counter B = d0's low word.
 * In:  d0 = the count; script; a6 = the wave object
 * Out: self->loop_b; NEXT
 * Evidence: move.w d0,$1C(a6) / NEXT; bra.s here from Op5_SetLoopB2 / B3 and Op5_SetLoopB5 (other
 *           function files). Prototype: no measured contract (a branch target only), from the
 *           callers' registers
 */
void Op5_SetLoopB_Store(uint16_t d0, uint32_t *handler, const u16 **script, Vm5Wave *self)
{
    self->loop_b = d0;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
