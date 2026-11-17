#include "game.h"

/* Op2_SeekWorldX8OrBranch - a2 op: Mot_SeekWorldXOrBranch with an 8-pixel tolerance.
 * In:  script = word branch offset
 * Out: tolerance = 8; then Mot_SeekWorldXOrBranch (jumped to)
 * Evidence: moveq #8,d1 / bra.s Mot_SeekWorldXOrBranch */
void Op2_SeekWorldX8OrBranch(uint16_t *d0, uint32_t *tolerance, uint32_t *handler, const u16 **script,
            const Vm2Motion *mot, const Vm2Obj *self)
{
    *tolerance = 8;
    dk::jump(at_Mot_SeekWorldXOrBranch);
}
