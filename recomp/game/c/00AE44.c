#include "game.h"

/* Scroll_SetMode - switches the V-int's scroll upload to another mode, with the V-int held off
 * around it; turns the H-int off first (a raster mode's H-int must not fire on the new mode).
 * In:  mode (d0.w) = an offset into VInt_ScrollModeTbl / VInt_BusyScrollTbl
 * Out: g_ScrollMode = mode; VDP reg 0 = $04 (H-int disabled), reg 10 = $FF
 * Evidence: addq.b #1,g_VIntOff / move.l #$80048AFF,$C00004 / move.w d0,g_ScrollMode / subq.b
 *           #1,g_VIntOff; callers in the stage-2 section code $033274, $033322, $033382 ...; 8
 *           corpus runs
 */
void Scroll_SetMode(uint16_t mode)
{
    g_VIntOff = (uint8_t)(g_VIntOff + 1);
    hw::port_write32(hw::VDP_CTRL, 0x80048AFFu);  /* reg 0 = $04, reg 10 = $FF */
    g_ScrollMode = mode;
    g_VIntOff = (uint8_t)(g_VIntOff - 1);
}
