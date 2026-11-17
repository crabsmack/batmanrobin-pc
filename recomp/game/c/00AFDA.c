#include "game.h"

/* VInt_BusyRearmRaster - busy-path entry for mode $28: re-arms the H-int raster on the table
 * already shown (no swap: the main loop didn't fill a new one).
 * In:  a0 = VDP control port
 * Out: as HInt_Rearm
 * Evidence: bra.w HInt_Rearm; the target of VInt_BusyScrollTbl28's slot $28; 5 corpus runs;
 *           V-int code, D2 checks it
 */
void VInt_BusyRearmRaster(uint32_t ctrl)
{
    (void)ctrl;
    dk::jump(at_HInt_Rearm);
}
