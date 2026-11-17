#include "game.h"

/* Op4_ForkRestAndEnd - a4 op: forks a child whose script is the rest of this one (the ops after
 * this word) and ends this run, so a parent that resumes here emits one child per frame.
 * In:  script (no operands); d2-d5 = the spawn registers; a6 = the spawner
 * Out: a3 = the script; then VM4_Fork's (falling into it; its rts ends the run)
 * Evidence: movea.l a4,a3, falling into VM4_Fork; 36 script uses, e.g. $14A1E (`60C2 6418 FFFF
 *           0001 4A2C 634E 67EE`: the child sets its animation and loops Op4_LoadXY / Op4_Draw); 129
 *           corpus runs
 */
void Op4_ForkRestAndEnd(uint32_t *d0, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t *a0,
                        uint32_t *a1, uint32_t *a3, uint32_t a4, uint32_t *a5, uint32_t a6)
{
    (void)d0, (void)d2, (void)d3, (void)d4, (void)d5, (void)a0, (void)a1, (void)a5, (void)a6;
    *a3 = a4;
    dk::jump(at_VM4_Fork);
}
