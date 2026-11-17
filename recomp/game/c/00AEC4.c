#include "game.h"

/* VInt_BusyScrollTbl - the busy-path scroll jump table's slot 0: the display back on.
 * In:  a0 = VDP control port
 * Out: as VInt_ScrollDisplayOn (reg 1 = $64)
 * Evidence: bra.w $AF04, entered by VInt_BusyScroll's jmp; the slot for mode 4 ($AEC8) is never
 *           read; 18 corpus runs; V-int code, D2 checks it
 */
void VInt_BusyScrollTbl(uint32_t ctrl)
{
    (void)ctrl;
    dk::jump(at_VInt_ScrollDisplayOn);
}
