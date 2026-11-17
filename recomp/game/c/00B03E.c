#include "game.h"

/* Scroll_DMALinesA - DMAs plane A's per-line H-scroll only: 224 words of g_HScrollLinesA to VRAM
 * $BC00, the auto-increment at 4, then back to 2.
 * In:  a0 = VDP control port, a2 = the 4-byte command scratch in RAM
 * Out: VDP reg 15 = 2
 * Evidence: Scroll_DMALines' first half: move.w #$8F04,(a0) / $93E09400 / $95E996F0 / $977F /
 *           move.l #$7C000082,(a2) / move.w (a2),(a0) / move.w 2(a2),(a0) / move.w #$8F02,(a0);
 *           from VInt_ScrollLinesARaster and VInt_ScrollLinesA; V-int code, D2 checks it
 */
void Scroll_DMALinesA(uint32_t ctrl, uint32_t scratch)
{
    PTR(u32) at;
    at.v = scratch;
    u32 *cmd = at;
    hw::port_write16(ctrl, 0x8F04);
    hw::port_write32(ctrl, 0x93E09400u);
    hw::port_write32(ctrl, 0x95E996F0u);
    hw::port_write16(ctrl, 0x977F);
    *cmd = 0x7C000082u;
    hw::port_write16(ctrl, (uint16_t)(*cmd >> 16));
    hw::port_write16(ctrl, (uint16_t)*cmd);
    hw::port_write16(ctrl, 0x8F02);
}
