#include "game.h"

enum {
    Z80_CHANNELS = 0x1480,           /* the driver's six channel records (Z80 address) */
    SND_CHANNEL_SIZE = 0x67,
    SND_CHANNEL_COUNT = 6,
    SNDCH_ON = 0x1F,                 /* a record's on flag ($FF from Snd_InitSongChannel) */
};

/* Snd_StopChannelsBody - with the Z80 bus held, clears the on flag of all six Z80 channel records
 * In:  none
 * Out: Z80 RAM $1480 + $67n + $1F = 0; the Z80 bus released; d7 = $FFFF (dbra)
 * Evidence: move.w #$100,$A11100 / btst #0 wait / lea $A01480,a1 / moveq #5,d7 / move.b #0,d0 /
 *           move.b d0,$1F(a1) / cmp.b $1F(a1),d0 / bne / adda.w #$67,a1 / dbra /
 *           move.w #0,$A11100 / rts; entered via Snd_StopChannels; 146 corpus runs
 */
uint32_t Snd_StopChannelsBody(void)
{
    hw::port_write16(hw::Z80_BUSREQ, 0x100);
    while (hw::port_read8(hw::Z80_BUSREQ) & 1) {
    }
    uint32_t channel = hw::Z80_RAM + Z80_CHANNELS;
    for (int i = 0; i < SND_CHANNEL_COUNT; i++, channel += SND_CHANNEL_SIZE) {
        do
            hw::port_write8(channel + SNDCH_ON, 0);
        while (hw::port_read8(channel + SNDCH_ON) != 0);
    }
    hw::port_write16(hw::Z80_BUSREQ, 0);
    return 0x0000FFFFu;   /* moveq #5,d7 counted down by dbra */
}
