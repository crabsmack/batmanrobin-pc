#include "game.h"

/* the Zyrinx Z80 program (ROM), copied whole to Z80 RAM $0000 */
static constexpr uint32_t Snd_1E6104 = 0x1E6104u;
enum { SND_Z80_PROGRAM_SIZE = 0x1825 };

/* Z80 addresses the 68000 side writes */
enum {
    Z80_DRIVER_PARAM = 0x003B,       /* Snd_LoadDriverBody's d1 byte (its meaning not traced) */
    Z80_BANK_POINTERS = 0x0E61,      /* two 3-byte bank pointers from the module header */
};

/* a Z80 channel record's bytes (offsets from its base, $1480 + $67n) */
enum {
    SNDCH_ON = 0x1F,                 /* $FF = the channel plays, 0 = off */
    SNDCH_DEF0 = 0x20,               /* per-channel bytes 1 and 2 (Snd_1E6104 + $1810 + 3n) */
    SNDCH_DEF1 = 0x21,
    SNDCH_HDR1 = 0x29,               /* channel header byte 1 */
    SNDCH_DEF2 = 0x2A,               /* per-channel byte 0 */
    SNDCH_HDR0 = 0x2B,               /* channel header byte 0 (its length / 4) */
    SNDCH_BANK = 0x2D,               /* the 32 KB ROM bank of the channel's data */
    SNDCH_WINDOW_HI = 0x2E,          /* the data's address in the Z80's bank window ($8000-$FFFF) */
    SNDCH_WINDOW_LO = 0x2F,
    SNDCH_FLAG = 0x3D,               /* $FF with the channel on, 0 off */
};

/* a byte into Z80 RAM, stored again until it reads back (every driver write but the bank
   pointers) */
static void z80_store(uint32_t at, uint8_t v)
{
    do
        hw::port_write8(at, v);
    while (hw::port_read8(at) != v);
}

/* Snd_InitSongChannel - fills one Z80 channel record from a song's channel header: the bank and
 * window address of its data, the two header bytes, three per-channel bytes, and the on flags $FF;
 * a header whose byte 0 is 0 switches the channel off instead
 * In:  d0 = the header's 68000 address (the same as header (a0)), channel (a1) = the Z80 record
 *      ($A01480 + $67n), per_channel (a3) = the next three per-channel bytes
 * Out: header (a0) = d0 = the next channel header (header + 4 * byte 0 + 2); d1 = 4 * byte 0;
 *      per_channel += 3 when the channel is on
 * Evidence: moveq #0,d1 / move.b (a0),d1 / lsl.w #2,d1 / beq (off) / movea.l d0,a2 / addq.l #2,d0 /
 *           lsl.l #1,d0 / lsr.w #1,d0 / addi.w #$8000,d0 -> +$2F, lsr.l #8 -> +$2E, lsr.w #8 ->
 *           +$2D; (a2)+ -> +$2B, (a2) -> +$29; 3 x (a3)+ -> +$2A/+$20/+$21; $FF -> +$3D, +$1F;
 *           adda.w d1,a0; off: d1.b -> +$1F, +$3D; then addq.w #2,a0 / move.l a0,d0; each byte
 *           with a read-back loop; called 6 times by Snd_StartSongBody; 146 corpus runs
 */
void Snd_InitSongChannel(uint32_t *d0, uint32_t *d1, uint32_t *header, uint32_t channel, uint32_t *per_channel)
{
    const u8 *hdr = snd_rom<u8>(*header);
    uint16_t length = (uint16_t)(hdr[0] << 2);
    *d1 = length;
    if (length == 0) {
        *d0 &= 0xFFFFFF00u;          /* move.b d1,d0 */
        z80_store(channel + SNDCH_ON, 0);
        z80_store(channel + SNDCH_FLAG, 0);
    } else {
        const u8 *src = snd_rom<u8>(*d0);   /* a2: the header again, through d0 */
        /* the data after the 2-byte header, as the Z80 reaches it: a 32 KB bank + its window */
        uint32_t data = *d0 + 2u;
        uint8_t bank = (uint8_t)(data >> 15);
        uint16_t window = (uint16_t)(0x8000u | (data & 0x7FFFu));
        z80_store(channel + SNDCH_WINDOW_LO, (uint8_t)window);
        z80_store(channel + SNDCH_WINDOW_HI, (uint8_t)(window >> 8));
        z80_store(channel + SNDCH_BANK, bank);
        z80_store(channel + SNDCH_HDR0, src[0]);
        z80_store(channel + SNDCH_HDR1, src[1]);
        const u8 *def = snd_rom<u8>(*per_channel);
        z80_store(channel + SNDCH_DEF2, def[0]);
        z80_store(channel + SNDCH_DEF0, def[1]);
        z80_store(channel + SNDCH_DEF1, def[2]);
        *per_channel += 3u;
        z80_store(channel + SNDCH_FLAG, 0xFF);
        z80_store(channel + SNDCH_ON, 0xFF);
        *header += (uint32_t)(int32_t)(int16_t)length;   /* adda.w d1,a0 */
    }
    *header += 2u;
    *d0 = *header;
}

/* Snd_StartSongFinish - tail of Snd_StartSongBody: copies the module header's two 3-byte bank
 * pointers (bytes 7-9 and 13-15, each in the order 7, 9, 8) to Z80 RAM $0E61-$0E66, then pulses the
 * Z80 reset and releases the bus, so the driver restarts with the new song
 * In:  the module's Tbl_1E7EE4 entry, pushed by Snd_StartSongBody (popped here)
 * Out: the Z80 running; a0 = the module header + 16, a1 = $A00E65
 * Evidence: movea.l (a7)+,a0 / lea $A00E61,a1 / movea.l 4(a0),a0 / addq.w #7,a0 /
 *           move.b (a0)+,(a1)+ / move.b (a0)+,1(a1) / move.b (a0)+,(a1) / addq.w #3,a0 /
 *           addq.w #2,a1 / the same three / move.w #0,$A11200 / move.w #0,$A11100 /
 *           move.w #$100,$A11200 / rts (no read-back here); reached by bra.w from
 *           Snd_StartSongBody; 146 corpus runs
 */
void Snd_StartSongFinish(uint32_t *a0, uint32_t *a1)
{
    const SndModule *module = snd_rom<SndModule>(dk::pop_long());
    const u8 *src = module->header;
    uint32_t dst = hw::Z80_RAM + Z80_BANK_POINTERS;
    for (int i = 0; i < 2; i++, src += 6, dst += 3) {
        hw::port_write8(dst + 0, src[7]);
        hw::port_write8(dst + 2, src[8]);
        hw::port_write8(dst + 1, src[9]);
    }
    /* the registers as the copy leaves them: the second pointer's end, its last Z80 byte */
    *a0 = module->header.raw() + 16u;
    *a1 = hw::Z80_RAM + Z80_BANK_POINTERS + 4u;
    hw::port_write16(hw::Z80_RESET, 0);
    hw::port_write16(hw::Z80_BUSREQ, 0);
    hw::port_write16(hw::Z80_RESET, 0x100);
}

/* Snd_LoadDriverBody - copies the Zyrinx Z80 program to Z80 RAM $0000 byte by byte with read-back,
 * stores d1 at Z80 $003B, then pulses the Z80 reset and releases the bus
 * In:  param (d1.b) = the byte for Z80 RAM $003B ($E4 from Snd_Init)
 * Out: the Z80 restarted with the driver; a5 = the end of the program in ROM ($1E7929)
 * Evidence: lea Snd_1E6104(pc),a5 / lea $A00000,a0 / move.w #$100,$A11100 / move.w #$100,$A11200 /
 *           btst #0 wait / move.w #$1824,d2 / move.b (a5)+,d0 / move.b d0,(a0) / cmp.b (a0),d0 /
 *           bne / addq.w #1,a0 / dbra / lea $A0003B,a0 / d1 with read-back / move.w #0,$A11200 /
 *           move.w #0,$A11100 / move.w #$100,$A11200 / rts; entered via Snd_LoadDriver; 146
 *           corpus runs
 */
uint32_t Snd_LoadDriverBody(uint8_t param)
{
    const u8 *program = snd_rom<u8>(Snd_1E6104);
    hw::port_write16(hw::Z80_BUSREQ, 0x100);
    hw::port_write16(hw::Z80_RESET, 0x100);
    while (hw::port_read8(hw::Z80_BUSREQ) & 1) {
    }
    for (uint32_t i = 0; i < SND_Z80_PROGRAM_SIZE; i++)
        z80_store(hw::Z80_RAM + i, program[i]);
    z80_store(hw::Z80_RAM + Z80_DRIVER_PARAM, param);
    hw::port_write16(hw::Z80_RESET, 0);
    hw::port_write16(hw::Z80_BUSREQ, 0);
    hw::port_write16(hw::Z80_RESET, 0x100);
    return Snd_1E6104 + SND_Z80_PROGRAM_SIZE;
}
