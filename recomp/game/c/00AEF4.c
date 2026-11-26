#include "game.h"

/* VInt_BusyMode30 - busy slot $30: the display back on, no scroll upload.
 * In:  a0 = VDP control port
 * Out: reg 1 = $64 (VInt_ScrollDisplayOn)
 * Evidence: bra.w $AF04 at $00AEF4; VInt_BusyScrollTbl offset $30, a mode no store sets; never run in the corpus
 */
void VInt_BusyMode30(uint32_t ctrl)
{
    (void)ctrl;
    dk::jump(at_VInt_ScrollDisplayOn);
}
