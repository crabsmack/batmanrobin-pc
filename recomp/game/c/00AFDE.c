#include "game.h"

/* Scroll_DMALinePairs - DMAs 448 words from $FFE1D2 (224 interleaved plane A / B line H-scroll
 * pairs) to the H-scroll table at VRAM $BC00.
 * In:  a0 = VDP control port, a2 = the 4-byte command scratch in RAM (the DMA-starting command is
 *      sent from RAM, as two words)
 * Out: none
 * Evidence: move.l #$93C09401,(a0) (length $1C0) / move.l #$95E996F0,(a0) / move.w #$977F,(a0)
 *           (source $FFE1D2) / move.l #$7C000082,(a2) / move.w (a2),(a0) / move.w 2(a2),(a0); only
 *           from VInt_ScrollLinePairs; V-int code, D2 checks it
 */
void Scroll_DMALinePairs(uint32_t ctrl, uint32_t scratch)
{
    PTR(u32) at;
    at.v = scratch;
    u32 *cmd = at;
    hw::port_write32(ctrl, 0x93C09401u);
    hw::port_write32(ctrl, 0x95E996F0u);
    hw::port_write16(ctrl, 0x977F);
    *cmd = 0x7C000082u;                           /* VRAM $BC00, DMA */
    hw::port_write16(ctrl, (uint16_t)(*cmd >> 16));
    hw::port_write16(ctrl, (uint16_t)*cmd);       /* the second word starts the DMA */
}
