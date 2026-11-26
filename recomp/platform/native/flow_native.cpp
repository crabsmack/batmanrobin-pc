/* flow_native.cpp - the native flow runtime's host on the native machine (phase 5 P6; ruling R8).
 * It replaces the phase-5 flow_host.cpp, the adapter to the engine's machine: what the flow runtime
 * (game/runtime/flow.cpp) asks of its platform (dk_flow_rt.h Host) is answered here, with nothing of
 * the 68000 or the engine under it.
 *   - the stack (and the exception vectors in ROM): game memory through dk_mem.h, wherever the
 *     memory layer keeps work RAM (ruling R1's arena);
 *   - a game function's entry: the machine's sync point (machine.cpp nm_entry);
 *   - IRQ_WAIT: the machine's frame-synchronous V-int (nm_wait_vint);
 *   - RAM code: the game's H-int handler at $FFE560 in C (game/runtime/hint_raster.cpp);
 *   - the reset: the register record as the 68000 leaves the reset exception (SSP from the vector
 *     table, SR $2704 as the reference reports it after reset, the rest 0), then the reset code;
 *   - interrupts: the machine says when (dk::flow::interrupt does the 68000's side).
 */
#include <stdint.h>

#include "dk_mem.h"
#include "runtime/dk_flow_rt.h"
#include "native_machine.h"

extern "C" int dk_ram_code(uint32_t addr);   /* game/runtime/hint_raster.cpp */

using dk::flow::g_regs;

static uint16_t mem_read16(uint32_t a) { return *AT68(const u16, a); }
static uint32_t mem_read32(uint32_t a) { return *AT68(const u32, a); }
static void mem_write16(uint32_t a, uint16_t v) { *AT68(u16, a) = v; }
static void mem_write32(uint32_t a, uint32_t v) { *AT68(u32, a) = v; }

static void entry(uint32_t addr) { nm_entry(addr); }
static void irq_wait(void) { nm_wait_vint(); }

extern "C" void dk_native_start(uint32_t entry_pc)
{
    for (int i = 0; i < 8; i++) g_regs.d[i] = 0;
    for (int i = 0; i < 8; i++) g_regs.a[i] = 0;
    const uint8_t *v = (const uint8_t *)dk::rom(0);   /* the reset vector's SSP (address 0: no pointer) */
    g_regs.a[7] = (uint32_t)v[0] << 24 | (uint32_t)v[1] << 16 | (uint32_t)v[2] << 8 | v[3];
    g_regs.sr = 0x2704u;           /* supervisor, mask 7, Z: as the reference reports after reset */
    g_regs.usp = 0;
    dk::flow::Host h = { mem_read16, mem_read32, mem_write16, mem_write32, entry, irq_wait, dk_ram_code };
    dk::flow::install(h);
    dk::flow::reset(entry_pc);
}

extern "C" void dk_native_interrupt(unsigned level, uint32_t pc) { dk::flow::interrupt(level, pc); }
extern "C" uint16_t dk_native_sr(void) { return g_regs.sr; }

/* A branch snapshot: the flow runtime keeps its own copy (one snapshot at a time, as the runner's
   branch plans take them); its run loops are on the game's fiber stack, which the machine saves. */
extern "C" size_t dk_native_flow_save(void *dst, size_t cap)
{
    (void)cap;
    if (dst) dk::flow::branch_save();
    return 0;
}

extern "C" void dk_native_flow_load(const void *src)
{
    (void)src;
    dk::flow::branch_restore();
}

/* ... and into a caller's buffer (state.cpp's "flow" section: quick saves and rewind, many states
   held at once) */
extern "C" size_t dk_native_flow_state_bytes(void) { return dk::flow::state_bytes(); }
extern "C" void dk_native_flow_state_save(void *dst) { dk::flow::state_save(dst); }
extern "C" void dk_native_flow_state_load(const void *src) { dk::flow::state_load(src); }
