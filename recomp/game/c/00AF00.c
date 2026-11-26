#include "game.h"

/* VInt_BusyMode3C - busy slot $3C: re-arms the H-int raster, as slot $38.
 * In:  a0 = VDP control port
 * Out: as HInt_Rearm (its condition codes)
 * Evidence: bra.w $B1D8 at $AF00; the stage-4 scenes set mode $3C ($03BB88); never run in the corpus
 */
void VInt_BusyMode3C(uint32_t ctrl)
{
    (void)ctrl;
    dk::jump(at_HInt_Rearm);
}
