#include "game.h"
#include "platform.h"

/* the standard init's table (ROM, right after Boot_Reset): three words (d5-d7), five port
   addresses (a0-a4), the 24 VDP register values, the VRAM fill command, the Z80 stub program, two
   VDP commands, the VSRAM command and the four PSG mute bytes, read in that order */
static constexpr uint32_t Tbl_00038E = 0x00038Eu;

enum {
    BOOT_VDP_REGS = 24,
    BOOT_Z80_STUB_BYTES = 38,
    BOOT_RAM_LONGS = 0x4000,    /* all 64 KB of work RAM */
    BOOT_CRAM_LONGS = 32,       /* 64 colours */
    BOOT_VSRAM_LONGS = 20,      /* 40 scroll words */
    BOOT_PSG_BYTES = 4,
    BOOT_SR = 0x2700,           /* supervisor, interrupts masked */
};

/* a reader over the init table, as the asm's (a5)+ */
struct BootTable {
    const u8 *p;
    uint8_t byte() { return *p++; }
    uint16_t word() { uint16_t v = (uint16_t)(p[0] << 8 | p[1]); p += 2; return v; }
    uint32_t lng() { uint32_t v = (uint32_t)word() << 16; return v | word(); }
};

/* Boot_Reset - the reset vector: on a power-on (the I/O control registers still 0) runs the
 * standard Sega init (TMSS, the 24 VDP registers, a VRAM fill, the Z80 stub, the 64 KB RAM clear,
 * CRAM / VSRAM clear, PSG mute), then every register from the cleared RAM and SR $2700; a soft
 * reset skips it. Then the region check (a jump: never returns).
 * In:  every register (a soft reset hands them on unchanged)
 * Out: power-on: d0-d7 / a0-a6 = the first 15 longs of RAM (0 after the clear), usp = 0, SR =
 *      $2700; then Boot_CheckRegion
 * Evidence: tst.l $A10008 / bne / tst.w $A1000C / bne Boot_CheckRegion; lea Tbl_00038E,a5 /
 *           movem.w d5-d7 / movem.l a0-a4; move.b -$10FF(a1) (the version register) / andi.b #$F
 *           / move.l #'SEGA',$2F00(a1); move.w (a4),d0 / moveq #0,d0 / movea.l d0,a6 / move.l
 *           a6,usp; the register loop (move.b (a5)+,d5 / move.w d5,(a4) / add.w d7,d5); the fill;
 *           the Z80 bus and reset lines around the stub copy (btst d0,(a1) waits for the bus);
 *           move.l d0,-(a6) x $4000 (from 0 down: a6 ends at $FFFF0000); the CRAM / VSRAM clears;
 *           move.b (a5)+,$11(a3) x 4; movem.l (a6),d0-d7/a0-a6 / move.w #$2700,sr; vector 1, TAS
 *           frame 1 (D2 checks it; no caller to shadow-test)
 */
void Boot_Reset(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5,
                uint32_t *a6)
{
    if (hw::port_read32(hw::IO_CTRL12_LONG) != 0 || hw::port_read16(hw::IO_CTRL3_WORD) != 0) {
        dk::jump(at_Boot_CheckRegion);   /* a soft reset: the registers as they are */
        return;
    }
    PTR(const u8) tbl;
    tbl.v = Tbl_00038E;
    BootTable t{ tbl };
    uint16_t reg = t.word();            /* d5: $8000, the register-write command */
    t.word();                           /* d6: the RAM clear's count ($3FFF), a fixed loop here */
    uint16_t step = t.word();           /* d7: $100, the next register / the Z80 lines' value */
    uint32_t z80_ram = t.lng(), busreq = t.lng(), z80_reset = t.lng(), vdp_data = t.lng(), vdp_ctrl = t.lng();

    if (hw::port_read8(hw::IO_VERSION) & 0xF)
        hw::port_write32(hw::TMSS_SEGA, 0x53454741u);   /* 'SEGA' */
    (void)hw::port_read16(vdp_ctrl);                   /* move.w (a4),d0: clears the command latch */
    dk::set_usp(0);
    for (int i = 0; i < BOOT_VDP_REGS; i++) {
        reg = (uint16_t)((reg & 0xFF00u) | t.byte());
        hw::port_write16(vdp_ctrl, reg);
        reg = (uint16_t)(reg + step);
    }
    hw::port_write32(vdp_ctrl, t.lng());               /* VRAM fill from 0 (DMA set by the registers) */
    hw::port_write16(vdp_data, 0);                     /* the fill value: starts it */
    hw::port_write16(busreq, step);                    /* the Z80's bus requested, its reset released */
    hw::port_write16(z80_reset, step);
    while (hw::port_read8(busreq) & 1) {
    }
    for (int i = 0; i < BOOT_Z80_STUB_BYTES; i++)
        hw::port_write8(z80_ram + (uint32_t)i, t.byte());
    hw::port_write16(z80_reset, 0);
    hw::port_write16(busreq, 0);
    hw::port_write16(z80_reset, step);

    u32 *ram = (u32 *)g_SceneBuf;                      /* move.l d0,-(a6) from the top down */
    for (int i = BOOT_RAM_LONGS; i-- > 0;)
        ram[i] = 0u;
    hw::port_write32(vdp_ctrl, t.lng());               /* two register writes (auto-increment) */
    hw::port_write32(vdp_ctrl, t.lng());               /* CRAM write at 0 */
    for (int i = 0; i < BOOT_CRAM_LONGS; i++)
        hw::port_write32(vdp_data, 0);
    hw::port_write32(vdp_ctrl, t.lng());               /* VSRAM write at 0 */
    for (int i = 0; i < BOOT_VSRAM_LONGS; i++)
        hw::port_write32(vdp_data, 0);
    for (int i = 0; i < BOOT_PSG_BYTES; i++)
        hw::port_write8(hw::PSG, t.byte());            /* each channel's volume off */
    hw::port_write16(z80_reset, 0);

    /* movem.l (a6),d0-d7/a0-a6: a6 = $FFFF0000 after the clear, the start of RAM */
    const u32 *saved = (const u32 *)g_SceneBuf;
    uint32_t *regs[15] = { d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6 };
    for (int i = 0; i < 15; i++)
        *regs[i] = saved[i];
    dk::set_status_register(BOOT_SR);
    dk::jump(at_Boot_CheckRegion);
}
