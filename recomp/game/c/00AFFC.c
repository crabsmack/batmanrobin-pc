#include "game.h"

/* one plane's line H-scroll DMA: 224 words (length regs $93E0/$9400) from the source regs, the
   DMA-starting command sent from the RAM scratch as two words */
static void dma_lines(uint32_t ctrl, u32 *cmd, uint32_t source_regs, uint32_t command)
{
    hw::port_write32(ctrl, 0x93E09400u);
    hw::port_write32(ctrl, source_regs);
    hw::port_write16(ctrl, 0x977F);
    *cmd = command;
    hw::port_write16(ctrl, (uint16_t)(*cmd >> 16));
    hw::port_write16(ctrl, (uint16_t)*cmd);
}

/* Scroll_DMALines - DMAs both planes' per-line H-scroll: g_HScrollLinesA to VRAM $BC00 and
 * g_HScrollLinesB to $BC02, 224 words each, with the auto-increment at 4 (one plane's word per
 * line), then back to 2.
 * In:  a0 = VDP control port, a2 = the 4-byte command scratch in RAM
 * Out: VDP reg 15 = 2
 * Evidence: move.w #$8F04,(a0); $93E0/$9400, $95E9/$96F0/$977F (source $FFE1D2), $7C000082 via
 *           (a2); $95C9/$96F1 (source $FFE392), $7C020082; move.w #$8F02,(a0); from
 *           VInt_ScrollLines/LinesColumns/LinesRaster and $24576; V-int code, D2 checks it
 */
void Scroll_DMALines(uint32_t ctrl, uint32_t scratch)
{
    PTR(u32) at;
    at.v = scratch;
    hw::port_write16(ctrl, 0x8F04);
    dma_lines(ctrl, at, 0x95E996F0u, 0x7C000082u);   /* g_HScrollLinesA -> $BC00 */
    dma_lines(ctrl, at, 0x95C996F1u, 0x7C020082u);   /* g_HScrollLinesB -> $BC02 */
    hw::port_write16(ctrl, 0x8F02);
}
