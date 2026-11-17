/* platform_native.cpp - the platform layer on the native machine (genesis_platform.h, backend
 * `native`, phase 5 P6): the native runner's hand-written C reaches the hardware through these six
 * calls and nothing else, and they go to the native machine's devices (platform/native/machine.cpp:
 * the engine's video model genesis_vdp.c, the Z80 with its RAM and bus/reset lines, the YM2612's
 * timers, the PSG, the I/O chip and the pads), with no 68000 and no emulated bus under them.
 *
 * Each call is the one 68000 access it stands for, in the original's order, width and address
 * (the same contract as the bus backend, so the native machine sees what the emulated runners'
 * devices saw); a long access is two word accesses, high word first, as the 68000 makes it. A
 * port device_of() does not know is fatal, as on the bus backend, where P1/P2 proved the list
 * complete.
 */
#include "genesis_platform.h"
#include "native/native_machine.h"

namespace dk {
[[noreturn]] void fatal(const char *fmt, ...);
}

namespace hw {

const char *device_name(Device d)
{
    switch (d) {
    case Device::vdp_data: return "VDP data";
    case Device::vdp_ctrl: return "VDP control";
    case Device::vdp_hv: return "VDP H/V counter";
    case Device::psg: return "PSG";
    case Device::io: return "I/O";
    case Device::z80_busreq: return "Z80 bus request";
    case Device::z80_reset: return "Z80 reset";
    case Device::tmss: return "TMSS";
    case Device::z80_ram: return "Z80 RAM";
    case Device::none: break;
    }
    return "none";
}

static inline uint32_t checked(uint32_t port, int size, char kind)
{
    if (device_of(port) == Device::none)
        dk::fatal("hw::port_%s%d: $%08X is not a hardware port the platform knows (genesis_platform.h device_of)",
                  kind == 'r' ? "read" : "write", size * 8, (unsigned)port);
    return port & 0xFFFFFFu;
}

uint8_t port_read8(uint32_t port) { return nm_bus_read8(checked(port, 1, 'r')); }
uint16_t port_read16(uint32_t port) { return nm_bus_read16(checked(port, 2, 'r')); }

uint32_t port_read32(uint32_t port)
{
    uint32_t a = checked(port, 4, 'r');
    uint32_t hi = nm_bus_read16(a);
    return hi << 16 | nm_bus_read16(a + 2u);
}

void port_write8(uint32_t port, uint8_t v) { nm_bus_write8(checked(port, 1, 'w'), v); }
void port_write16(uint32_t port, uint16_t v) { nm_bus_write16(checked(port, 2, 'w'), v); }

void port_write32(uint32_t port, uint32_t v)
{
    uint32_t a = checked(port, 4, 'w');
    nm_bus_write16(a, (uint16_t)(v >> 16));
    nm_bus_write16(a + 2u, (uint16_t)v);
}

}  /* namespace hw */
