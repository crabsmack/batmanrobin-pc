#include "game.h"

/* VInt_BusyScrollTbl8 ($00AECC-$00AED7): the busy-path scroll jump table's modes 8-$10, all three
 * the display back on with no scroll upload. V-int code: D2 checks it. */

/* VInt_BusyScrollTbl8 - busy slot 8: the display back on.
 * In:  a0 = VDP control port
 * Out: reg 1 = $64 (VInt_ScrollDisplayOn)
 * Evidence: bra.w $AF04 at $AECC, a computed target of the jmp at $AEC0 in 10 corpus runs
 */
void VInt_BusyScrollTbl8(uint32_t ctrl)
{
    (void)ctrl;
    dk::jump(at_VInt_ScrollDisplayOn);
}

/* VInt_BusyMode0C - busy slot $C: the display back on.
 * In:  a0 = VDP control port
 * Out: reg 1 = $64 (VInt_ScrollDisplayOn)
 * Evidence: bra.w $AF04 at $AED0; 82 corpus runs
 */
void VInt_BusyMode0C(uint32_t ctrl)
{
    (void)ctrl;
    dk::jump(at_VInt_ScrollDisplayOn);
}

/* VInt_BusyMode10 - busy slot $10: the display back on.
 * In:  a0 = VDP control port
 * Out: reg 1 = $64 (VInt_ScrollDisplayOn)
 * Evidence: bra.w $AF04 at $AED4; 8 corpus runs
 */
void VInt_BusyMode10(uint32_t ctrl)
{
    (void)ctrl;
    dk::jump(at_VInt_ScrollDisplayOn);
}
