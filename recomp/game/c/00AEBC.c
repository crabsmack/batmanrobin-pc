#include "game.h"

/* VInt_BusyScroll - the busy-path V-int's scroll step (the main loop hadn't finished its frame):
 * continues at the slot of VInt_BusyScrollTbl that g_ScrollMode selects; the slots only turn
 * the display back on, or also re-arm the H-int raster, so the frame's scroll isn't uploaded.
 * In:  a0 = VDP control port; g_ScrollMode
 * Out: as the slot's routine (reg 1 = $64)
 * Evidence: move.w g_ScrollMode,d0 / jmp VInt_BusyScrollTbl(pc,d0.w); only from VInt_BusyPath;
 *           86 TAS frames; runs in the V-int, so D2 checks it
 */
void VInt_BusyScroll(uint32_t ctrl)
{
    (void)ctrl;
    dk::jump(at_VInt_BusyScrollTbl + (int16_t)(uint16_t)g_ScrollMode);
}
