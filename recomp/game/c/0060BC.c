#include "game.h"

/* Op4_ForkAndEnd - a4 op: forks a child running the script at the operand's address plus its value
 * and ends this run (Op4_Fork's child script, Op4_ForkAbsAndEnd's end).
 * In:  script = word child-script offset; d2-d5; a6 = the spawner
 * Out: a3 = the child's script; then VM4_Fork's (its rts ends the run)
 * Evidence: movea.l a4,a3 / adda.w (a4)+,a3 / bra.s VM4_Fork; Scr4_036F88 at $036FA6; never run in the corpus.
 *           Prototype: Op4_ForkAbsAndEnd's (the same tail)
 */
void Op4_ForkAndEnd(uint32_t *d0, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t *a0, uint32_t *a1,
                    uint32_t *a3, const u16 **script, uint32_t *a5, uint32_t a6)
{
    (void)d0, (void)d2, (void)d3, (void)d4, (void)d5, (void)a0, (void)a1, (void)a5, (void)a6;
    *a3 = dk::addr(*script) + (uint32_t)(int32_t)(int16_t)(uint16_t)**script;
    (*script)++;
    dk::jump(at_VM4_Fork);
}
