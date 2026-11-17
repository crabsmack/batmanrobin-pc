#include "game.h"

/* Op4_ForkOrBranch - a4 op: forks a child a4 object (VM4_Fork: the script at the operand's
 * address plus its value, the motion from d2-d5) and continues; when the pool is full it
 * branches instead.
 * In:  script = word child-script offset (from its own word), word branch offset taken on
 *      failure; d2-d5 = the spawn radius / heading / x / y (16.16); a6 = the spawner
 * Out: a3 = the child's script; a5 = the child; VM4_Fork's d0 / a0 / a1 and condition codes;
 *      NEXT past the operands (Op4_SkipWord), or the failure branch
 * Evidence: movea.l a4,a3 / adda.w (a4)+,a3 / bsr.w VM4_Fork / beq.w Op4_SkipWord / adda.w (a4),a4 /
 *           NEXT; script uses $26D5C (`607C 001E 0002`), $37026 (`607C 0062 0044`); 67 corpus runs
 */
void Op4_ForkOrBranch(uint32_t *d0, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t *a0, uint32_t *a1,
                      uint32_t *a3, const u16 **script, uint32_t *a5, const Vm4Obj *self, dk::Flags *ccr)
{
    *a3 = dk::addr(*script) + (uint32_t)(int32_t)(int16_t)(uint16_t)**script;
    (*script)++;
    VM4_Fork(d0, d2, d3, d4, d5, a0, a1, *a3, a5, self, ccr);
    if (ccr->z) {
        dk::jump(at_Op4_SkipWord);
        return;
    }
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    *a0 = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*a0);
}
