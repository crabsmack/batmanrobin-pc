#include "game.h"

/* VInt_BusyMode18 - busy slot $18: the display back on, no scroll upload.
 * In:  a0 = VDP control port
 * Out: reg 1 = $64 (VInt_ScrollDisplayOn)
 * Evidence: bra.w $AF04 at $00AEDC; VInt_BusyScrollTbl offset $18, a mode no store sets; never run in the corpus
 */
void VInt_BusyMode18(uint32_t ctrl)
{
    (void)ctrl;
    dk::jump(at_VInt_ScrollDisplayOn);
}
