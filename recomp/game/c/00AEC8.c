#include "game.h"

/* VInt_BusyMode04 - busy slot $04: the display back on, no scroll upload.
 * In:  a0 = VDP control port
 * Out: reg 1 = $64 (VInt_ScrollDisplayOn)
 * Evidence: bra.w $AF04 at $00AEC8; VInt_BusyScrollTbl offset $04, a mode the stage-4 scenes set ($03BB88); never run in the corpus
 */
void VInt_BusyMode04(uint32_t ctrl)
{
    (void)ctrl;
    dk::jump(at_VInt_ScrollDisplayOn);
}
