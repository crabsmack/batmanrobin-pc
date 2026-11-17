#include "game.h"
#include "platform.h"

/* The command word that starts a DMA has to come from RAM (the VDP starts the transfer on that
 * write), so the V-int stages the command in its scratch (g_VDPCmdBuf, kept in a2) and sends its
 * two words from there. */
static void send_staged(uint32_t ctrl, u16 *buf, uint32_t command)
{
    buf[0] = (uint16_t)(command >> 16);       /* move.l #command,(a2) */
    buf[1] = (uint16_t)command;
    hw::port_write16(ctrl, buf[0]);
    hw::port_write16(ctrl, buf[1]);
}

/* Scroll_DMALinesB - DMAs plane B's per-line H-scroll only: 224 words of g_HScrollLinesB to VRAM
 * $BC02 (every other word of the H-scroll table), auto-increment 4, then the increment back to 2.
 * In:  ctrl = the VDP control port (a0); scratch = the V-int's command buffer g_VDPCmdBuf (a2)
 * Out: buf = the DMA command $7C020082; VDP reg 15 = 2
 * Evidence: $8F04, $93E0/$9400 = $E0 words, $95C9/$96F1/$977F = source $FFE392 (g_HScrollLinesB
 *           >> 1), command $7C020082 = VRAM $BC02 + DMA, $8F02; from VInt_ScrollLinesBRaster;
 *           16 corpus runs
 */
void Scroll_DMALinesB(uint32_t ctrl, uint32_t scratch)
{
    PTR(u16) staging;
    staging.v = scratch;
    u16 *buf = staging;
    hw::port_write16(ctrl, 0x8F04);
    hw::port_write32(ctrl, 0x93E09400u);      /* length $00E0 words */
    hw::port_write32(ctrl, 0x95C996F1u);      /* source g_HScrollLinesB */
    hw::port_write16(ctrl, 0x977F);           /* ... a 68000-memory DMA */
    send_staged(ctrl, buf, 0x7C020082u);
    hw::port_write16(ctrl, 0x8F02);
}
