#include "game.h"
#include "platform.h"

/* The command word that starts a DMA has to come from RAM, so the V-int stages it in its scratch
 * (g_VDPCmdBuf, kept in a2) and sends its two words from there. */
static void send_staged(uint32_t ctrl, u16 *buf, uint32_t command)
{
    buf[0] = (uint16_t)(command >> 16);       /* move.l #command,(a2) */
    buf[1] = (uint16_t)command;
    hw::port_write16(ctrl, buf[0]);
    hw::port_write16(ctrl, buf[1]);
}

/* Scroll_DMAColumns - DMAs 40 words of 2-cell column V-scroll (20 interleaved plane A/B pairs)
 * from $FF9000 to VSRAM 0, at the auto-increment the caller left (2).
 * In:  ctrl = the VDP control port (a0); scratch = the V-int's command buffer g_VDPCmdBuf (a2)
 * Out: buf = the DMA command $40000090
 * Evidence: $9328/$9400 = $28 words, $9500/$96C8/$977F = source $FF9000, command $40000090 =
 *           VSRAM write 0 + DMA; called by the H-int jmp-stub setups $244A8 and $24576 (stage 2);
 *           20 corpus runs
 */
void Scroll_DMAColumns(uint32_t ctrl, uint32_t scratch)
{
    PTR(u16) staging;
    staging.v = scratch;
    u16 *buf = staging;
    hw::port_write32(ctrl, 0x93289400u);      /* length $0028 words */
    hw::port_write32(ctrl, 0x950096C8u);      /* source $FF9000 */
    hw::port_write16(ctrl, 0x977F);
    send_staged(ctrl, buf, 0x40000090u);
}
