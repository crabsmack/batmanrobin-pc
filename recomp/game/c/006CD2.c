#include "game.h"

/* Op5_SetLoopB5 - a5 op: sets loop counter B to 5 (the 4 and 6-8 entries around it never ran and
 * stay raw bytes).
 * In:  script (no operands); a6 = the wave object
 * Out: d0 = 5; then Op5_SetLoopB_Store (self->loop_b, NEXT)
 * Evidence: moveq #5,d0 / bra.s Op5_SetLoopB_Store; script use $21F74 (`6CD2 6C62`); 1 corpus run
 */
void Op5_SetLoopB5(uint32_t *d0, uint32_t *a0, uint32_t *a5, uint32_t a6)
{
    (void)a0, (void)a5, (void)a6;
    *d0 = 5;
    dk::jump(at_Op5_SetLoopB_Store);
}
