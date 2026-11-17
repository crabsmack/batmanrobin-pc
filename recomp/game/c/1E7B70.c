#include "game.h"

/* the three bytes per channel copied into each channel record (ROM: inside the Z80 program's
   image, Snd_1E6104 + $1810) */
static constexpr uint32_t Snd_ChannelDefaults = 0x1E7914u;

enum {
    Z80_CHANNELS = 0x1480,           /* the driver's six channel records (Z80 address) */
    SND_CHANNEL_SIZE = 0x67,
    SND_CHANNEL_COUNT = 6,
};

/* Snd_StartSongBody - stops the Z80 (bus and reset), finds song d0's header through the module's
 * 3-byte song pointers and fills the six channel records from it; Snd_StartSongFinish then loads
 * the bank pointers and restarts the Z80
 * In:  module (a0) = the module's Tbl_1E7EE4 entry, song (d0) low word = the song in the module
 * Out: d0 = the address after the last channel header, d1 = its header length (Snd_InitSongChannel's),
 *      d7 = $FFFF (dbra); module pushed for Snd_StartSongFinish (a tail jump)
 * Evidence: move.l a0,-(a7) / lea Snd_1E6104+$1810(pc),a3 / move.w #$100,$A11100 /
 *           move.w #$100,$A11200 / btst #0 wait / movea.l (a0),a0 / adda.w d0,a0 / add.w d0,d0 /
 *           adda.w d0,a0 / 24-bit big-endian pointer into d0 and a0 / lea $A01480,a1 / moveq #5,d7 /
 *           bsr.w Snd_InitSongChannel / adda.w #$67,a1 / dbra / bra.w Snd_StartSongFinish; entered
 *           via Snd_StartSong; 146 corpus runs
 */
void Snd_StartSongBody(uint32_t *song, uint32_t *d1, uint32_t *d7, const SndModule *module)
{
    dk::push_long(ADDR68(module));   /* popped by Snd_StartSongFinish */
    uint32_t per_channel = Snd_ChannelDefaults;
    hw::port_write16(hw::Z80_BUSREQ, 0x100);
    hw::port_write16(hw::Z80_RESET, 0x100);   /* the Z80 held in reset while the records change */
    while (hw::port_read8(hw::Z80_BUSREQ) & 1) {
    }
    /* the song's 3-byte pointer: songs + 3 * song (two word adds, each sign-extended) */
    uint16_t n = (uint16_t)*song;
    uint32_t at = module->songs.raw() + (uint32_t)(int32_t)(int16_t)n;
    n = (uint16_t)(n + n);
    at += (uint32_t)(int32_t)(int16_t)n;
    const u8 *p = snd_rom<u8>(at);
    uint32_t header = ((uint32_t)p[0] << 16) | ((uint32_t)p[1] << 8) | p[2];
    uint32_t d0 = header;
    uint32_t channel = hw::Z80_RAM + Z80_CHANNELS;
    for (int i = 0; i < SND_CHANNEL_COUNT; i++, channel += SND_CHANNEL_SIZE)
        Snd_InitSongChannel(&d0, d1, &header, channel, &per_channel);
    *song = d0;
    *d7 = 0x0000FFFFu;   /* moveq #5 counted down by dbra */
    dk::jump(at_Snd_StartSongFinish);
}
