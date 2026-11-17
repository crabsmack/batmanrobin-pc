/* genesis_platform.h - the platform layer (phase 5, criterion P4: docs/phases/P5-port.md): the
 * Genesis hardware the hand-written C touches, by name, and the small C API it touches it through.
 * The game's C (recomp/game/c, recomp/game/include) never names a hardware address; it includes
 * this header through game/include/platform.h.
 *
 * The API is shaped like the hardware ports: one call per 68000 access, in the original's order,
 * width and address. The game keeps port addresses in registers (the tile-queue writer gets
 * a0 = the VDP control port, a1 = the data port; the sound driver's mailbox is an address into Z80
 * RAM), so a port is a value: the 68000 address of the register (the names below), and the
 * platform decodes it (device_of). A long write is the 68000's move.l: on the VDP ports, two word
 * writes, high word first, which the backend performs as the bus does.
 *
 * Backends (one per runner, chosen at link time in recomp/CMakeLists.txt, game_handc PLATFORM):
 *   bus     platform_bus.cpp: the engine's emulated bus (m68k_read* / m68k_write*), as the
 *           68000's own accesses: the stepped, stepped-gen and verify runners (verify compares each
 *           C call's hardware access sequence with the generated code's, so nothing here may
 *           reorder, merge or widen an access).
 *   native  platform_native.cpp (P6): the native runner's machine (platform/native/machine.cpp):
 *           the same calls on the VDP model (genesis_vdp.c, R3), the Z80 with its RAM, bus request
 *           and reset lines, the YM2612's timers, the PSG and the pads, no 68000. It checks
 *           device_of() as the bus backend does, which proved the device list complete (P1/P2).
 *
 * The interrupt mask and the V-int wait are the native flow runtime's in the native build
 * (dk::set_interrupt_mask -> dk::flow, IRQ_WAIT -> dk::flow::irq_wait: P3, ruling R8), which asks
 * the platform for the wait (platform/native/flow_native.cpp: the machine's V-int).
 */
#pragma once
#include <stdint.h>

namespace hw {

/* --- the hardware names (68000 addresses; byte registers of the I/O chip at odd addresses) ---- */

/* The VDP ports. Data: VRAM/CRAM/VSRAM through the address the last command set. Control: commands,
   register writes ($8000 | reg << 8 | value), DMA; a read returns the status word. */
static constexpr uint32_t VDP_DATA = 0xC00000u;
static constexpr uint32_t VDP_CTRL = 0xC00004u;
/* The PSG's port (a byte write: a latch / volume command) */
static constexpr uint32_t PSG = 0xC00011u;

/* The I/O chip (Boot_CheckRegion, Boot_InitIOPorts): the version register (bits 7-6 = the
   console's region, the low nibble the hardware version), the three ports' control registers
   (bit 6 = TH as output) and their serial control registers. */
static constexpr uint32_t IO_VERSION = 0xA10001u;
static constexpr uint32_t IO_CTRL1 = 0xA10009u;
static constexpr uint32_t IO_CTRL2 = 0xA1000Bu;
static constexpr uint32_t IO_CTRL3 = 0xA1000Du;
static constexpr uint32_t IO_SCTRL1 = 0xA10013u;
static constexpr uint32_t IO_SCTRL2 = 0xA10019u;
static constexpr uint32_t IO_SCTRL3 = 0xA1001Fu;
/* the pad ports' data registers (Pad_ReadAll / Pad_ReadLive: bit 6 = TH, the select line) */
static constexpr uint32_t IO_DATA1 = 0xA10003u;
static constexpr uint32_t IO_DATA2 = 0xA10005u;
/* Boot_Reset's cold-start test: a long read of ports 1-2's control registers ($A10008-$A1000B)
   and a word read of port 3's ($A1000C): all 0 only after a power-on (a soft reset keeps them) */
static constexpr uint32_t IO_CTRL12_LONG = 0xA10008u;
static constexpr uint32_t IO_CTRL3_WORD = 0xA1000Cu;
/* TMSS: consoles with a version number (the version register's low nibble) need 'SEGA' written
   here (a long write) before the VDP works */
static constexpr uint32_t TMSS_SEGA = 0xA14000u;

/* The Z80's bus request and reset lines (VInt_Handler: $100 requests the bus / releases reset, 0
   gives it back; bit 0 of the request register's byte reads 1 until the bus is granted). */
static constexpr uint32_t Z80_BUSREQ = 0xA11100u;
static constexpr uint32_t Z80_RESET = 0xA11200u;
/* The Z80's 8 KB RAM as the 68000 sees it (the sound driver's program and variables: Snd_ bodies,
   with the bus held): byte accesses, port_write8 / port_read8 at Z80_RAM + the Z80 address. */
static constexpr uint32_t Z80_RAM = 0xA00000u;
static constexpr uint32_t Z80_RAM_SIZE = 0x2000u;

/* --- the API (out of line: the backend's object, never inlined into a game object) ------------- */

uint8_t port_read8(uint32_t port);
uint16_t port_read16(uint32_t port);
uint32_t port_read32(uint32_t port);
void port_write8(uint32_t port, uint8_t v);
void port_write16(uint32_t port, uint16_t v);
void port_write32(uint32_t port, uint32_t v);

/* --- the decode both backends share ------------------------------------------------------------ */

enum class Device : uint8_t {
    none,        /* not a port the game uses: fatal */
    vdp_data,    /* $C00000-$C00003 */
    vdp_ctrl,    /* $C00004-$C00007 */
    vdp_hv,      /* $C00008-$C0000F, the H/V counter */
    psg,         /* $C00011, $C00013, $C00015, $C00017 */
    io,          /* $A10000-$A1001F, the I/O chip's registers */
    z80_busreq,  /* $A11100-$A11101 */
    z80_reset,   /* $A11200-$A11201 */
    tmss,        /* $A14000-$A14003 */
    z80_ram,     /* $A00000-$A01FFF */
};

/* The device a port address names (the 68000's 24-bit bus: the top byte is ignored). */
constexpr Device device_of(uint32_t port)
{
    uint32_t a = port & 0xFFFFFFu;
    if (a >= 0xC00000u && a <= 0xC00003u) return Device::vdp_data;
    if (a >= 0xC00004u && a <= 0xC00007u) return Device::vdp_ctrl;
    if (a >= 0xC00008u && a <= 0xC0000Fu) return Device::vdp_hv;
    if (a >= 0xC00011u && a <= 0xC00017u && (a & 1u)) return Device::psg;
    if (a >= 0xA10000u && a <= 0xA1001Fu) return Device::io;
    if (a == 0xA11100u || a == 0xA11101u) return Device::z80_busreq;
    if (a == 0xA11200u || a == 0xA11201u) return Device::z80_reset;
    if (a >= TMSS_SEGA && a <= TMSS_SEGA + 3u) return Device::tmss;
    if (a >= Z80_RAM && a < Z80_RAM + Z80_RAM_SIZE) return Device::z80_ram;
    return Device::none;
}

const char *device_name(Device d);

}  /* namespace hw */
