#include "game.h"

/* Op2_SeekWorldY8OrBranch - a2 op: Mot_SeekWorldYOrBranch with an 8-pixel tolerance.
 * In:  script = word branch offset
 * Out: tolerance = 8; then Mot_SeekWorldYOrBranch (jumped to)
 * Evidence: moveq #8,d1 / bra.s Mot_SeekWorldYOrBranch
 */
void Op2_SeekWorldY8OrBranch(uint16_t *d0, uint32_t *tolerance, uint32_t *handler, const u16 **script,
            const Vm2Motion *mot, const Vm2Obj *self)
{
    *tolerance = 8;
    dk::jump(at_Mot_SeekWorldYOrBranch);
}
