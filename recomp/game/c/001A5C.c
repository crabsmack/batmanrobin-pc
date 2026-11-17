#include "game.h"

/* Op2_SeekScreenY4 - a2 op: Mot_SeekScreenY with a 4-pixel tolerance.
 * In:  script = -
 * Out: tolerance = 4; then Mot_SeekScreenY (jumped to)
 * Evidence: moveq #4,d1 / bra.s Mot_SeekScreenY
 */
void Op2_SeekScreenY4(uint16_t *d0, uint32_t *tolerance, uint32_t *handler, const u16 **script,
            const Vm2Motion *mot, const Vm2Obj *self)
{
    *tolerance = 4;
    dk::jump(at_Mot_SeekScreenY);
}
