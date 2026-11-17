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

/* Scroll_DMAColumnsAB - DMAs the column V-scroll of both planes from separate buffers: 20 words
 * from $FF9000 to VSRAM 0 (plane A's columns) and 20 from $FF9028 to VSRAM 2 (plane B's), each
 * every other VSRAM word (auto-increment 4), then the increment back to 2.
 * In:  ctrl = the VDP control port (a0); scratch = the V-int's command buffer g_VDPCmdBuf (a2)
 * Out: buf = the second DMA command $40020090; VDP reg 15 = 2
 * Evidence: $8F04, $9314/$9400 = $14 words, sources $FF9000 / $FF9028 ($9500 / $9514, $96C8,
 *           $977F), commands $40000090 / $40020090, $8F02; only from VInt_ScrollLinesColumns; 14
 *           corpus runs
 */
void Scroll_DMAColumnsAB(uint32_t ctrl, uint32_t scratch)
{
    PTR(u16) staging;
    staging.v = scratch;
    u16 *buf = staging;
    hw::port_write16(ctrl, 0x8F04);
    hw::port_write32(ctrl, 0x93149400u);      /* length $0014 words */
    hw::port_write32(ctrl, 0x950096C8u);      /* source $FF9000 */
    hw::port_write16(ctrl, 0x977F);
    send_staged(ctrl, buf, 0x40000090u);      /* VSRAM 0 */
    hw::port_write32(ctrl, 0x93149400u);
    hw::port_write32(ctrl, 0x951496C8u);      /* source $FF9028 */
    hw::port_write16(ctrl, 0x977F);
    send_staged(ctrl, buf, 0x40020090u);      /* VSRAM 2 */
    hw::port_write16(ctrl, 0x8F02);
}
