#include "game.h"

/* VInt_BusyScrollTbl34 ($00AEF8-$00AEFF): the busy-path scroll jump table's modes $34-$38: both
 * re-arm the H-int raster, so the per-line effect survives a dropped frame. V-int code: D2
 * checks it. */

/* VInt_BusyScrollTbl34 - busy slot $34: re-arms the H-int raster.
 * In:  a0 = VDP control port
 * Out: as HInt_Rearm
 * Evidence: bra.w $B1D8 at $AEF8; 15 corpus runs, TAS frames 39894-46233 (stage 2)
 */
void VInt_BusyScrollTbl34(uint32_t ctrl)
{
    (void)ctrl;
    dk::jump(at_HInt_Rearm);
}

/* VInt_BusyMode38 - busy slot $38: re-arms the H-int raster.
 * In:  a0 = VDP control port
 * Out: as HInt_Rearm (its condition codes)
 * Evidence: bra.w $B1D8 at $AEFC; 11 corpus runs
 */
void VInt_BusyMode38(uint32_t ctrl)
{
    (void)ctrl;
    dk::jump(at_HInt_Rearm);
}
