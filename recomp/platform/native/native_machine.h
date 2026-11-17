/* native_machine.h - the native runner's own machine (phase 5 P6: docs/phases/P5-port.md): what its
 * parts say to each other. No 68000 here: the game is the hand-written C on the native flow runtime
 * (game/runtime/flow.cpp, ruling R8), and this machine is the hardware it talks to through the
 * platform layer (platform/genesis_platform.h, backend platform_native.cpp).
 *
 *   machine.cpp        the frame-synchronous machine (ruling R5, with R3's raster): the VDP's ports
 *                      on the engine's video model (genesis_vdp.c), DMA, the I/O chip and the pads,
 *                      the Z80 with its RAM, bus and reset lines, the YM2612's timers, the PSG, the
 *                      V-int at the game's wait, the H-int raster, the sync point, the game's fiber;
 *   flow_native.cpp    the flow runtime's host (in the native game library: dk_mem.h for the stack);
 *   game_mem.cpp       work RAM as the machine reads it (DMA, the Z80's window, branch snapshots),
 *                      through dk_mem.h: wherever the memory layer keeps it (ruling R1's arena);
 *   runner.cpp         main(): the ROM, the command line the kit drives, the sync rows from the
 *                      exporter, branch plans, the picture hash, the counters; the played game;
 *   input_script.cpp   the pads per runner frame (--input-script);
 *   host_sdl.cpp       the window, the keyboard and controllers, the sound device (played runs).
 */
#ifndef NATIVE_MACHINE_H
#define NATIVE_MACHINE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- machine.cpp ---- */
void nm_power_on(void);                   /* the machine at power-on, the game's fiber made (not run) */
void nm_run_frame(void);                  /* one runner frame: the Z80's frame, then the game to its next sync point */
int  nm_game_stopped(void);               /* the game's fiber ended (a fatal returns through exit) */
void nm_set_render(int on);               /* the picture drawn (configuration, not state) */
void nm_set_play(int on);                 /* played: each extra V-int of an interval is a host frame */
int  nm_extra_frame(void);                /* the last nm_run_frame stopped at such a V-int, not a sync point */
void nm_set_pads(uint16_t p1, uint16_t p2);   /* GPAD_* masks, before a frame */
void nm_set_pad_types(int t1, int t2);    /* 0 = 3-button, 1 = 6-button, 2 = none */
uint64_t nm_sync_count(void);             /* sync points so far (volatile read: the watchdog) */
uint64_t nm_progress(void);               /* sync points + IRQ_WAIT V-ints so far: game time (the watchdog) */
uint32_t nm_last_entry(void);             /* the last game function entered (diagnostics) */
const uint32_t *nm_framebuffer(int *stride, int *w, int *h);   /* ARGB8888, the frame's active region */
/* branch plans: the whole machine (the game's fiber with the C frames on it, work RAM, the video
   model, the Z80, the machine's own state, the flow runtime) into a buffer and back */
size_t nm_snapshot_save(uint8_t **buf);   /* malloc'd; the caller keeps it */
int    nm_snapshot_load(const uint8_t *buf, size_t len);

/* the 68000's hardware accesses (platform_native.cpp, after device_of): one call per access, the
   port's 68000 address, as the engine's frame-synchronous bus takes them (exact_sched.c xbus_*) */
uint8_t  nm_bus_read8(uint32_t a);
uint16_t nm_bus_read16(uint32_t a);
void     nm_bus_write8(uint32_t a, uint8_t v);
void     nm_bus_write16(uint32_t a, uint16_t v);

/* called by the flow host (flow_native.cpp) */
void nm_entry(uint32_t addr);             /* a game function entered: the sync point at depth 0 */
void nm_wait_vint(void);                  /* IRQ_WAIT: the frame-synchronous V-int with its pads */

/* ---- flow_native.cpp (in the native game library) ---- */
void     dk_native_start(uint32_t entry_pc);           /* the reset: the flow runtime from entry_pc */
void     dk_native_interrupt(unsigned level, uint32_t pc);
uint16_t dk_native_sr(void);                           /* the register record's SR (its interrupt mask) */
size_t   dk_native_flow_save(void *dst, size_t cap);   /* the flow runtime's state (NULL: its size) */
void     dk_native_flow_load(const void *src);

/* ---- game_mem.cpp (in the native game library: dk_mem.h) ---- */
void     nm_ram_init(void);               /* the arena at power-on: zeroed, every byte plain */
uint8_t  nm_ram_read8(uint32_t a);        /* work RAM ($FFxxxx), as the 68000 reads it */
uint16_t nm_ram_read16(uint32_t a);
void     nm_ram_write8(uint32_t a, uint8_t v);
void     nm_ram_write16(uint32_t a, uint16_t v);   /* a word / long stored at its own width (a script poke) */
void     nm_ram_write32(uint32_t a, uint32_t v);
size_t   nm_ram_save(void *dst, size_t cap);   /* work RAM's whole state (NULL: its size) */
int      nm_ram_load(const void *src, size_t len);

/* ---- runner.cpp ---- */
extern uint8_t g_rom[0x400000];
void nm_on_sync(uint64_t sync, uint64_t frame);   /* the sync point's row (the exporter's image) */
void nm_game_start(void);                         /* on the game's fiber, before the reset */
void nm_stuck(const char *why);                   /* "[stepped] ...: stuck", the rows flushed, exit 3 */

/* ---- input_script.cpp ---- */
int      ns_load(const char *path);
int      ns_active(void);
int      ns_mutates_state(void);                  /* the script writes RAM (WRITE_RAM*) */
void     ns_tick(uint64_t frame);
uint16_t ns_player_mask(int player);
uint32_t ns_vpad_count(void);
uint16_t ns_vpad(uint32_t i, int player);
uint16_t ns_peek(uint32_t n, int player);
uint32_t ns_peek_frames(void);            /* how many reference frames ns_peek can still give */
int      ns_should_exit(void);
int      ns_exit_code(void);

#ifdef __cplusplus
}
#endif

#endif /* NATIVE_MACHINE_H */
