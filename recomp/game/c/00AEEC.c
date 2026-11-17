#include "game.h"

/* VInt_BusyScrollTbl28 ($00AEEC-$00AEF3): the busy-path scroll jump table's modes $28-$2C. V-int
 * code: D2 checks it. */

/* VInt_BusyScrollTbl28 - busy slot $28: re-arms the H-int raster on the table already shown.
 * In:  a0 = VDP control port
 * Out: as VInt_BusyRearmRaster
 * Evidence: bra.w $AFDA at $AEEC; a computed target of the jmp at $AEC0 in 5 corpus runs
 */
void VInt_BusyScrollTbl28(uint32_t ctrl)
{
    (void)ctrl;
    dk::jump(at_VInt_BusyRearmRaster);
}

/* VInt_BusyMode2C - busy slot $2C: only the display back on (the mode has no raster).
 * In:  a0 = VDP control port
 * Out: reg 1 = $64; the condition codes VInt_ScrollDisplayOn's move leaves
 * Evidence: bra.w $AF04 at $AEF0; 11 corpus runs
 */
void VInt_BusyMode2C(uint32_t ctrl)
{
    (void)ctrl;
    dk::jump(at_VInt_ScrollDisplayOn);
}
