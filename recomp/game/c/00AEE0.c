#include "game.h"

/* VInt_BusyMode1C - busy slot $1C: the display back on, no scroll upload.
 * In:  a0 = VDP control port
 * Out: reg 1 = $64 (VInt_ScrollDisplayOn)
 * Evidence: bra.w $AF04 at $00AEE0; VInt_BusyScrollTbl offset $1C, a mode no store sets; never run in the corpus
 */
void VInt_BusyMode1C(uint32_t ctrl)
{
    (void)ctrl;
    dk::jump(at_VInt_ScrollDisplayOn);
}
