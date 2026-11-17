/* platform_bus.cpp - the platform layer on the engine's emulated bus (genesis_platform.h, backend
 * `bus`): every call is the one 68000 access it stands for, through m68k_read* / m68k_write*, so
 * the engine's devices (and verify's hardware hook, which compares each C call's access sequence
 * with the generated code's) see exactly what the original's instruction did: same order, width
 * and address. The stepped and verify runners link it (the native
 * runner has its own machine: platform_native.cpp, phase 5 P6).
 *
 * A port no device answers (device_of() == none) is a fatal error: the native backend (P6)
 * implements exactly the devices listed in the header, so a port missing there must fail here first,
 * on the runners whose corpus passes (P1, P2) cover every access the C makes. */
#include "genesis_platform.h"

extern "C" {
uint8_t m68k_read8(uint32_t addr);
uint16_t m68k_read16(uint32_t addr);
uint32_t m68k_read32(uint32_t addr);
void m68k_write8(uint32_t addr, uint8_t v);
void m68k_write16(uint32_t addr, uint16_t v);
void m68k_write32(uint32_t addr, uint32_t v);
}

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
    return port;
}


uint8_t port_read8(uint32_t port) { return m68k_read8(checked(port, 1, 'r')); }
uint16_t port_read16(uint32_t port) { return m68k_read16(checked(port, 2, 'r')); }
uint32_t port_read32(uint32_t port) { return m68k_read32(checked(port, 4, 'r')); }
void port_write8(uint32_t port, uint8_t v) { m68k_write8(checked(port, 1, 'w'), v); }
void port_write16(uint32_t port, uint16_t v) { m68k_write16(checked(port, 2, 'w'), v); }
void port_write32(uint32_t port, uint32_t v) { m68k_write32(checked(port, 4, 'w'), v); }

}  /* namespace hw */
