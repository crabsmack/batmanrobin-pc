#include "game.h"

/* Op2_BranchTouchBox_KindC - the jump table's kind-$C slot: the enemy list, as kind 8.
 * In:  as Op2_BranchTouchBox_Kind0
 * Out: bra.w Op2_BranchTouchBox_Enemies
 * Evidence: bra.w Op2_BranchTouchBox_Enemies at $3756, the jmp at $3746 with d3 = $C; never run in
 *           the corpus. Prototype: Op2_BranchTouchBox_Kind8's (the same branch)
 */
void Op2_BranchTouchBox_KindC(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                              uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t a4,
                              uint32_t a5, uint32_t a6)
{
    dk::jump(at_Op2_BranchTouchBox_Enemies);
}
