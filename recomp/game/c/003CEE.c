#include "game.h"

/* Op2_BranchWeaponLvlMaxGE - a2 op: branches when g_WeaponLvlMax (the best weapon level among the
 * players in play) is at least the operand; the compare is Op2_BranchStatGE's.
 * In:  script = word level, word offset; g_WeaponLvlMax
 * Out: stat = g_WeaponLvlMax; then Op2_BranchStatGE (the branch, else NEXT after the operands)
 * Evidence: move.w (g_WeaponLvlMax).w,d0 / bra.s Op2_BranchStatGE; its never-run neighbours
 *           $3CDC-$3D4E load the other Player_GatherStats words; 38 corpus runs; $13FD4 `3CEE 0002 FFEC`
 */
void Op2_BranchWeaponLvlMaxGE(uint16_t *stat, uint32_t *handler, const u16 **script)
{
    (void)handler;
    (void)script;
    *stat = g_WeaponLvlMax;
    dk::jump(at_Op2_BranchStatGE);
}
