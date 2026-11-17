#include "game.h"

/* Op5_SetLoopB2 / Op5_SetLoopB3 ($006CC6-$006CCD): loop counter B set to a constant, through the
 * shared store Op5_SetLoopB_Store in the Op5_SetLoopB file. */

/* Op5_SetLoopB2 - a5 op: sets loop counter B to 2.
 * In:  script (no operands); a6 = the wave object
 * Out: d0 = 2; then Op5_SetLoopB_Store (self->loop_b, NEXT)
 * Evidence: moveq #2,d0 / bra.s Op5_SetLoopB_Store; 65 script uses, e.g. $22C80, $21E82; 5 corpus
 *           runs
 */
void Op5_SetLoopB2(uint32_t *d0, uint32_t *a0, uint32_t *a5, uint32_t a6)
{
    (void)a0, (void)a5, (void)a6;
    *d0 = 2;
    dk::jump(at_Op5_SetLoopB_Store);
}

/* Op5_SetLoopB3 - a5 op: sets loop counter B to 3.
 * In:  as Op5_SetLoopB2
 * Out: d0 = 3; then Op5_SetLoopB_Store
 * Evidence: moveq #3,d0 / bra.s Op5_SetLoopB_Store; script uses $22504, $22BA0; 5 corpus runs
 */
void Op5_SetLoopB3(uint32_t *d0, uint32_t *a0, uint32_t *a5, uint32_t a6)
{
    (void)a0, (void)a5, (void)a6;
    *d0 = 3;
    dk::jump(at_Op5_SetLoopB_Store);
}
