/* platform.h - the Genesis hardware the hand-written C touches: the platform layer's names and API
 * (phase 5, P4), which live in recomp/platform/genesis_platform.h (namespace hw: VDP_DATA,
 * VDP_CTRL, PSG, IO_*, TMSS_SEGA, Z80_BUSREQ, Z80_RESET, Z80_RAM; hw::port_read8/16/32 and
 * hw::port_write8/16/32, one call per 68000 access, out of line in the runner's backend). Add a
 * port there when a function first needs it; the game's C never names a hardware address. */
#pragma once
#include "genesis_platform.h"
