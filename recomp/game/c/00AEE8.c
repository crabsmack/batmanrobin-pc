#include "game.h"

/* VInt_BusyMode24 - busy slot $24: the display back on, no scroll upload.
 * In:  a0 = VDP control port
 * Out: reg 1 = $64 (VInt_ScrollDisplayOn)
 * Evidence: bra.w $AF04 at $00AEE8; VInt_BusyScrollTbl offset $24, a mode no store sets; never run in the corpus
 */
void VInt_BusyMode24(uint32_t ctrl)
{
    (void)ctrl;
    dk::jump(at_VInt_ScrollDisplayOn);
}
