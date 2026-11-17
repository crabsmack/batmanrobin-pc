/* machine.cpp - the native runner's machine (phase 5 P6, rulings R3, R5, R8, R9): the hardware behind
 * the platform layer, and the frame-synchronous schedule the game runs on, with no 68000.
 *
 * It is the engine's frame-synchronous machine (the stepped runners' exact_sched.c paths, the
 * reference every C build is compared with), reproduced rule for rule so the native runner's state
 * equals stepped-gen's at every sync point:
 *   - one runner frame per sync interval: the Z80 runs its frame (its V-int line held one line from
 *     where it is, then to the frame's end), then the game runs to its next sync point (an entry to
 *     DK_SYNC_PC in the main program), where its row is written and its fiber parks;
 *   - the V-int is taken at the game's wait for it (IRQ_WAIT: GameSpec.fsync_vint_at_wait), when
 *     enabled (VDP reg 1 bit 5) and unmasked (SR mask below 6); the first of an interval with the
 *     runner frame's pads, each further one with the next of the interval's later reference frames
 *     (the script's VPAD lines, then its later frames: spec.c wait_vint_here);
 *   - after each such V-int the display is drawn line by line (R3's raster, GameSpec.fsync_raster):
 *     the H-int counter (reg 10) reloads at the display's start, fires at the end of each line and
 *     vblank's first, and with H-int enabled (reg 0 bit 4) and the mask below 4 the game's H-int
 *     handler runs before the next line; a latch left pending is taken at the display's start;
 *   - DMA completes at once; the VDP status reads FIFO empty, never DMA-busy, its VBLANK bit
 *     alternating from read to read (clear first after a V-int); the H/V counter reads 0;
 *   - the Z80 runs 8 lines whenever the 68000 releases its bus; its YM2612 timers in samples of
 *     1,008 master clocks on the Z80's own clock;
 *   - a hang guard: 20,000,000 hardware reads with no sync point end the run.
 * Every rule depends only on the program's own hardware accesses, so the C on the flow runtime sees
 * the machine generated code saw.
 *
 * Adapted from segagenesisrecomp (runner/exact_sched.c's frame-synchronous mode, runner/video/
 * genesis_machine.c, runner/video/genesis_bus.c), PolyForm Noncommercial License 1.0.0, Copyright
 * (c) 2026 Matthew Stan (https://polyformproject.org/licenses/noncommercial/1.0.0). What the engine
 * does there for its emulated 68000 (instruction timing, FIFO timing, open-bus prefetch, replay
 * logs, its bus front, its scheduler fiber) is gone: the game's C calls the platform functions
 * here, and the 68000 side of an interrupt is the flow runtime's (dk::flow::interrupt). The video
 * model (genesis_vdp.c), the Z80 core (superzazu) and the sound chips are the engine's.
 *
 * What the original's hardware would answer but nothing here decodes (open-bus bits: the 68000's
 * prefetch) reads as 0: the game tests only bit 0 of the bus-grant byte. A Z80-area read without
 * the bus, a 68000 YM2612 access and a DMA from anywhere but ROM and work RAM are fatal: the
 * game makes none (P1 proves it on the corpus).
 */
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
#include "video/genesis_machine.h"
#include "video/genesis_dac.h"
#include "audio/event_queue.h"
#include "fiber_compat.h"
}

#include "dkit_sync.h"
#include "native_machine.h"
#include "options.h"
#include "enhance.h"
#include "spr_host.h"
#include "ws_draw.h"
#include "present.h"
#include "sound2.h"
#include "widescreen.h"

GenesisMachine g_machine;
extern "C" unsigned long g_snd_frame;   /* chip_trace.c: the sound's frame number (the mixer reads it) */
extern "C" unsigned g_snd_pcz;          /* chip_trace.c: a chip write's writer (trace attribution) */

#define MPL   3420u   /* master clocks per line */
#define LINES 262
#define V (&g_machine.vdp)
#define REG(n) (g_machine.vdp.reg[(n)])

[[noreturn]] static void machine_fatal(const char *fmt, ...)
{
    char msg[400];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(msg, sizeof msg, fmt, ap);
    va_end(ap);
    fprintf(stderr, "[native] machine: %s\n", msg);
    fflush(stderr);
    exit(5);
}

/* ======================================================================== */
/* The machine's state: every value the schedule and the devices keep between accesses, in one
   block (a branch snapshot copies it). The video model, the Z80 core and its RAM are g_machine's. */

struct MState {
    /* VDP ports (exact_sched.c) */
    uint8_t  hint_pending, vint_pending, pending;
    uint16_t addr_latch;
    int32_t  cached_write;
    uint32_t dma_length;
    int      dma_type;           /* 0 68K->VRAM, 1 68K->CRAM/VSRAM, 2 fill, 3 copy */
    int      dmafill;
    uint32_t dma_src;
    uint16_t fifo[4];
    int      fifo_idx;
    /* I/O */
    uint8_t  io_reg[16];
    uint8_t  pad_state[2], pad_counter[2], pad_timeout[2];
    /* Z80 */
    uint32_t zclk;               /* Z80 time in master clocks */
    int      zstate;             /* bit0 reset released, bit1 bus requested */
    int      zaccess;            /* the 68000 may access the Z80 area */
    /* YM2612 timers */
    uint32_t fm_count, fm_start;
    int      fm_ta, fm_tal, fm_tac, fm_tbl, fm_tbc;
    uint8_t  fm_mode, fm_status;
    uint16_t fm_addr;
    /* the frame-synchronous schedule */
    uint64_t frame_no;           /* runner frames run */
    uint64_t sync_no;            /* sync points */
    uint64_t fs_reads;           /* hardware reads since the last sync point (the hang guard) */
    int      fs_vblank;          /* the status shows VBLANK at the next read */
    int      depth;              /* interrupt handlers running (0: the main program) */
    uint32_t vints;              /* V-ints taken at waits since the last sync point */
    uint32_t stamp_base;         /* the Z80's audio stamp base (output only) */
    uint32_t last_entry;
};
static MState s;

/* Not in a snapshot, as the engine's own statics aren't (genesis_machine.c): the Z80's EI shadow
   and the time of its current instruction. A branch restored at a frame end keeps the shadow of
   the frame that ran before the restore, exactly as stepped-gen's branches do. */
static int s_ei_shadow;
static uint32_t s_znow, s_zstamp;

/* Configuration */
static int s_render = 1, s_play;
static int s_extra;              /* nm_run_frame stopped at an extra V-int (played runs) */

/* ======================================================================== */
/* The picture */

static uint32_t s_cram_argb[GVDP_OUTPUT_PALETTE_SIZE];
#define FB_STRIDE 512
#define FB_HEIGHT 480
static uint32_t s_fb[FB_STRIDE * FB_HEIGHT];

static void colour_cb(void *u, unsigned idx, uint16_t bgr9)
{
    (void)u;
    if (idx >= GVDP_CRAM_ENTRIES) return;
    s_cram_argb[idx]                          = genesis_dac_cram_to_argb(bgr9, 0);
    s_cram_argb[idx + GVDP_PALETTE_SHADOW]    = genesis_dac_cram_to_argb(bgr9, 1);
    s_cram_argb[idx + GVDP_PALETTE_HIGHLIGHT] = genesis_dac_cram_to_argb(bgr9, 2);
}

static void render_line(int line)
{
    static uint8_t idxbuf[GVDP_MAX_WIDTH];
    GVDP *v = V;
    int dbl = gvdp_interlace_double(v);
    for (int sub = 0; sub <= dbl; sub++) {
        int row = (line << dbl) + sub;
        int n = gvdp_render_scanline(v, row, idxbuf);
        const uint32_t *host_palette = gvdp_host_palette();
        if (host_palette)
            memcpy(s_cram_argb + GVDP_HOST_PALETTE_BASE, host_palette, GVDP_HOST_PALETTE_SIZE * sizeof(uint32_t));
        if (row < 0 || row >= FB_HEIGHT) continue;
        uint32_t *out = s_fb + (size_t)row * FB_STRIDE;
        int w = n < FB_STRIDE ? n : FB_STRIDE;
        for (int x = 0; x < w; x++) out[x] = s_cram_argb[idxbuf[x]];
    }
}

extern "C" const uint32_t *nm_palette(void) { return s_cram_argb; }   /* present.cpp's capture */

const uint32_t *nm_framebuffer(int *stride, int *w, int *h)
{
    *stride = FB_STRIDE;
    *w = gvdp_active_width(V);
    *h = gvdp_output_height(V);
    return s_fb;
}

/* ======================================================================== */
/* Game memory as the bus sees it (a DMA's source, the Z80's bank window) */

static uint8_t bus_peek8(uint32_t a, const char *who)
{
    a &= 0xFFFFFFu;
    if (a < 0x400000u) return g_rom[a];
    if (a >= 0xE00000u) return nm_ram_read8(0xFF0000u | (a & 0xFFFFu));
    if (a >= 0xA00000u && a < 0xA02000u) return g_machine.bus.z80_ram[a & 0x1FFFu];
    machine_fatal("%s read $%06X: not ROM, work RAM or Z80 RAM (not modelled; the game makes no such read)", who, (unsigned)a);
}

/* ======================================================================== */
/* YM2612 timers and status (the CPU-visible part; the sound is the engine's ymfm, from the queue) */

#define FM_SAMPLE 1008u

static void fm_update(uint32_t t)
{
    if (t <= s.fm_count) return;
    uint32_t n = (t - s.fm_count + FM_SAMPLE - 1) / FM_SAMPLE;
    for (uint32_t i = 0; i < n; i++) {
        if (s.fm_mode & 0x01) {
            if (--s.fm_tac <= 0) {
                if (s.fm_mode & 0x04) s.fm_status |= 0x01;
                s.fm_tac = s.fm_tal;
            }
        }
    }
    if (s.fm_mode & 0x02) {
        s.fm_tbc -= (int)n;
        if (s.fm_tbc <= 0) {
            if (s.fm_mode & 0x08) s.fm_status |= 0x02;
            if (s.fm_tbl) s.fm_tbc += s.fm_tbl; else s.fm_tbc = s.fm_tbl;
        }
    }
    s.fm_count += n * FM_SAMPLE;
}

static void fm_set_timers(uint8_t v)
{
    if ((v & 1) && !(s.fm_mode & 1)) s.fm_tac = s.fm_tal;
    if ((v & 2) && !(s.fm_mode & 2)) s.fm_tbc = s.fm_tbl;
    s.fm_status &= (uint8_t)(~v >> 4);
    s.fm_mode = v;
}

static void fm_reset_chip(uint32_t t)
{
    fm_update(t);
    s.fm_tac = s.fm_tbc = 0;
    fm_set_timers(0x30);
    s.fm_tbl = 256 << 4;
    s.fm_ta = 0;
    s.fm_tal = 1024;
}

static uint8_t ym_read(uint32_t t)
{
    fm_update(t);
    return s.fm_status;
}

static void ym_write(uint32_t t, unsigned port, uint8_t v)
{
    if (port & 1) fm_update(t);
    switch (port & 3) {
    case 0: s.fm_addr = v; break;
    case 2: s.fm_addr = (uint16_t)(v | 0x100); break;
    default:
        switch (s.fm_addr) {
        case 0x24: s.fm_ta = (s.fm_ta & 0x03) | (v << 2); s.fm_tal = 1024 - s.fm_ta; break;
        case 0x25: s.fm_ta = (s.fm_ta & 0x3FC) | (v & 3); s.fm_tal = 1024 - s.fm_ta; break;
        case 0x26: s.fm_tbl = (256 - v) << 4; break;
        case 0x27: fm_set_timers(v); break;
        default: break;
        }
        break;
    }
}

/* ======================================================================== */
/* The Z80: its bus (genesis_bus.c gbus_z80_*), its core in master clocks (genesis_machine.c) */

#define MASTER_PER_Z80 15u

static uint8_t z80_bus_read(uint16_t addr)
{
    GenesisBus *b = &g_machine.bus;
    if (addr < 0x4000u) {
        if ((g_snd_nosfx || g_snd_deaf) && (addr & 0x1FFFu) - 0x17EAu < 36u) return 0;   /* sound2.h: the reference without effects */
        return b->z80_ram[addr & 0x1FFFu];
    }
    if (addr < 0x6000u) return ym_read(s_znow);
    if (addr < 0x8000u) return 0xFFu;
    return bus_peek8(((uint32_t)b->z80_bank << 15) + (uint32_t)(addr - 0x8000u), "the Z80's bank window");
}

static void z80_bus_write(uint16_t addr, uint8_t val)
{
    GenesisBus *b = &g_machine.bus;
    if (addr < 0x4000u) {
        uint16_t off = addr & 0x1FFFu;
        if (off - 0x1480u < 0x180Eu - 0x1480u)   /* the channel records and the effect window: the counters */
            snd_m1_ram(off, b->z80_ram[off], val, g_machine.z80.pc);
        b->z80_ram[off] = val;
        return;
    }
    if (addr < 0x6000u) {
        uint8_t port = (uint8_t)(addr & 3);
        ym_write(s_znow, port, val);
        g_snd_pcz = (unsigned)g_machine.z80.pc;
        snd_m1_ym(s_zstamp, port, val, g_machine.z80.iy);   /* the event queue, or sound2.cpp's synthesis */
        return;
    }
    if (addr < 0x6100u) { b->z80_bank = ((b->z80_bank >> 1) | ((val & 1u) << 8)) & 0x1FFu; return; }
    if (addr == 0x7F11u) { g_snd_pcz = (unsigned)g_machine.z80.pc; audio_event_push(s_zstamp, AUDIO_PORT_PSG, val); return; }
    if (addr >= 0x8000u) {
        uint32_t a = (((uint32_t)b->z80_bank << 15) + (uint32_t)(addr - 0x8000u)) & 0xFFFFFFu;
        if (a < 0x400000u) return;                         /* ROM: the write goes nowhere */
        if (a >= 0xE00000u) { nm_ram_write8(0xFF0000u | (a & 0xFFFFu), val); return; }
        if (a >= 0xA00000u && a < 0xA02000u) { b->z80_ram[a & 0x1FFFu] = val; return; }
        machine_fatal("the Z80's bank window wrote $%06X: not modelled (the game's driver writes no such address)", (unsigned)a);
    }
}

static uint8_t z80_read(void *u, uint16_t a) { (void)u; return z80_bus_read(a); }
static void z80_write(void *u, uint16_t a, uint8_t v) { (void)u; z80_bus_write(a, v); }
static uint8_t z80_port_in(z80 *z, uint8_t port) { return z80_bus_read((uint16_t)((z->b << 8) | port)); }
static void z80_port_out(z80 *z, uint8_t port, uint8_t value) { z80_bus_write((uint16_t)((z->b << 8) | port), value); }

static void z80_own_reset(z80 *z)
{
    z->pc = 0;
    z->iff1 = z->iff2 = 0;
    z->halted = 0;
    z->interrupt_mode = 0;
    z->iff_delay = 0;
    z->int_pending = z->nmi_pending = 0;
    z->int_data = 0xFF;
}

static void z80_exact_reset(void)
{
    z80_own_reset(&g_machine.z80);
    g_machine.z80.i = 0;   /* the reset line also clears I and R */
    g_machine.z80.r = 0;
}

static void z80_power_on(void)
{
    z80 *z = &g_machine.z80;
    z->a = z->b = z->c = z->d = z->e = z->h = z->l = 0;
    z->a_ = z->b_ = z->c_ = z->d_ = z->e_ = z->h_ = z->l_ = z->f_ = 0;
    z->sp = z->ix = z->iy = 0;
    z->mem_ptr = 0;
    z->sf = 0; z->zf = 1; z->yf = 0; z->hf = 0; z->xf = 0; z->pf = 0; z->nf = 0; z->cf = 0;
    z80_exact_reset();
}

/* Run the Z80 to master clock `target`: a pending interrupt is taken before an instruction (not in
   an EI shadow), then one instruction runs (the reference's order). */
static void z80_step(uint32_t *zclk, uint32_t target)
{
    z80 *z = &g_machine.z80;
    while (*zclk < target) {
        s_zstamp = s.stamp_base + *zclk;
        if (!s_ei_shadow) {
            unsigned long c0 = z->cyc;
            if (z80_take_irq(z)) {
                *zclk += (uint32_t)(z->cyc - c0) * MASTER_PER_Z80;
                if (*zclk >= target) break;
            }
        }
        unsigned long c0 = z->cyc;
        s_znow = *zclk + z80_base_cycles(z) * MASTER_PER_Z80;
        s_ei_shadow = z80_exec_one(z);
        *zclk += (uint32_t)(z->cyc - c0) * MASTER_PER_Z80;
    }
}

static void z80_irq(int asserted) { g_machine.z80.int_pending = asserted ? 1 : 0; }

/* The 68000's bus request and reset lines (exact_sched.c, frame-synchronous: the Z80's own clock) */
static void z80_busreq_w(int req)
{
    uint32_t now = s.zclk;
    if (req) {
        if (s.zstate == 1) s.zaccess = 1;
        s.zstate |= 2;
    } else {
        if (s.zstate == 3) { s.zclk = now; s.zaccess = 0; }
        s.zstate &= 1;
        /* a released Z80 runs a fixed slice, so a 68000 waiting for the sound driver's answer
           sees it after a deterministic number of polls */
        if (s.zstate == 1) z80_step(&s.zclk, s.zclk + 8u * MPL);
    }
    g_machine.bus.z80_busreq = (uint8_t)((s.zstate & 2) ? 1 : 0);
    snd_busreq(req);   /* machine 2's line (music_voices) */
}

static void z80_reset_w(int release)
{
    uint32_t now = s.zclk;
    if (release) {
        if (s.zstate == 0) { s.zclk = now; z80_exact_reset(); fm_reset_chip(now); }
        else if (s.zstate == 2) { s.zaccess = 1; z80_exact_reset(); fm_reset_chip(now); }
        s.zstate |= 1;
    } else {
        if (s.zstate == 3) s.zaccess = 0;
        fm_reset_chip(now);
        s.zstate &= 2;
    }
    g_machine.bus.z80_reset_off = (uint8_t)(s.zstate & 1);
    snd_reset(release);   /* machine 2's line (music_voices) */
}

/* machine 1's Z80 side for sound2.cpp (machine 2 started as a copy) */
extern "C" void nm_sound_m1(SndM1 *o)
{
    o->z80 = &g_machine.z80;
    o->ram = g_machine.bus.z80_ram;
    o->bank = g_machine.bus.z80_bank;
    o->zclk = s.zclk; o->stamp_base = s.stamp_base;
    o->zstate = s.zstate; o->zaccess = s.zaccess; o->ei_shadow = s_ei_shadow;
    o->fm_count = s.fm_count; o->fm_start = s.fm_start;
    o->fm_ta = s.fm_ta; o->fm_tal = s.fm_tal; o->fm_tac = s.fm_tac; o->fm_tbl = s.fm_tbl; o->fm_tbc = s.fm_tbc;
    o->fm_mode = s.fm_mode; o->fm_status = s.fm_status; o->fm_addr = s.fm_addr;
    o->vints = s.vints;
}

/* ======================================================================== */
/* VDP ports (exact_sched.c, frame-synchronous: DMA at once, no FIFO timing) */

static uint32_t dma_length_reg(void)
{
    uint32_t n = ((uint32_t)REG(20) << 8) | REG(19);
    return n ? n : 0x10000u;
}

/* The VDP's internal 16-bit write: one FIFO entry, then the store. */
static void vdp_bus_w(uint16_t data)
{
    GVDP *v = V;
    s.fifo[s.fifo_idx] = data;
    s.fifo_idx = (s.fifo_idx + 1) & 3;
    switch (v->code & 0x0F) {
    case 0x01:
        if (g_ws_window) { ws_vram_touch((uint16_t)(v->address & 0xFFFE)); ws_vram_touch((uint16_t)(v->address | 1)); }
        gvdp_store_vram_word(v, v->address, data);
        break;
    case 0x03: gvdp_store_cram(v, v->address, data); break;
    case 0x05: gvdp_store_vsram(v, v->address, data); break;
    default: break;
    }
    v->address = (uint16_t)(v->address + REG(15));
}

static void dma_68k(uint32_t words)
{
    uint32_t source = ((uint32_t)REG(23) << 17) | (s.dma_src << 1);
    do {
        uint16_t data;
        uint32_t a = source & 0xFFFFFFu;
        if (a >= 0xA00000u && a < 0xA20000u)   /* I/O area: the Z80 window reads work RAM */
            data = (uint16_t)((nm_ram_read8(0xFF0000u | (source & 0xFFFFu)) << 8) | nm_ram_read8(0xFF0000u | ((source + 1) & 0xFFFFu)));
        else if (a >= 0xE00000u)
            data = nm_ram_read16(0xFF0000u | (a & 0xFFFFu));
        else
            data = (uint16_t)((bus_peek8(a, "a DMA") << 8) | bus_peek8(a + 1u, "a DMA"));
        source += 2;
        source = ((uint32_t)REG(23) << 17) | (source & 0x1FFFFu);
        vdp_bus_w(data);
    } while (--words);
    s.dma_src = (source >> 1) & 0xFFFFu;
}

static void dma_copy(uint32_t bytes)
{
    GVDP *v = V;
    if (!(v->code & 0x10)) return;
    uint16_t source = (uint16_t)s.dma_src;
    do {
        if (g_ws_window) ws_vram_touch((uint16_t)(v->address ^ 1));
        v->vram[(uint16_t)(v->address ^ 1)] = g_ws_window ? ws_vram_game8((uint16_t)(source ^ 1)) : v->vram[(uint16_t)(source ^ 1)];
        gvdp_sat_byte(v, (uint16_t)(v->address ^ 1), v->vram[(uint16_t)(v->address ^ 1)]);
        source++;
        v->address = (uint16_t)(v->address + REG(15));
    } while (--bytes);
    s.dma_src = source;
}

static void dma_fill(uint32_t bytes)
{
    GVDP *v = V;
    switch (v->code & 0x0F) {
    case 0x01: {
        uint8_t data = (uint8_t)(s.fifo[(s.fifo_idx + 3) & 3] >> 8);
        do {
            if (g_ws_window) ws_vram_touch((uint16_t)(v->address ^ 1));
            v->vram[(uint16_t)(v->address ^ 1)] = data;
            gvdp_sat_byte(v, (uint16_t)(v->address ^ 1), data);
            v->address = (uint16_t)(v->address + REG(15));
        } while (--bytes);
        break;
    }
    case 0x03: {
        uint16_t data = s.fifo[s.fifo_idx];
        do { gvdp_store_cram(v, v->address, data); v->address = (uint16_t)(v->address + REG(15)); } while (--bytes);
        break;
    }
    case 0x05: {
        uint16_t data = s.fifo[s.fifo_idx];
        do { gvdp_store_vsram(v, v->address, data); v->address = (uint16_t)(v->address + REG(15)); } while (--bytes);
        break;
    }
    default:
        do { v->address = (uint16_t)(v->address + REG(15)); } while (--bytes);
        break;
    }
}

static void vdp_ctrl_w(uint16_t data);

static void dma_finished(void)
{
    uint16_t end = (uint16_t)(REG(21) + (REG(22) << 8) + REG(19) + (REG(20) << 8));
    REG(21) = (uint8_t)end;
    REG(22) = (uint8_t)(end >> 8);
    REG(19) = REG(20) = 0;
    if (s.cached_write >= 0) {
        uint16_t w = (uint16_t)s.cached_write;
        s.cached_write = -1;
        vdp_ctrl_w(w);
    }
}

/* No clock: the whole transfer happens at its start. */
static void dma_update(void)
{
    uint32_t n = s.dma_length;
    s.dma_length = 0;
    if (n) {
        switch (s.dma_type) {
        case 0: case 1: dma_68k(n); break;
        case 2:         dma_fill(n); break;
        default:        dma_copy(n); break;
        }
    }
    dma_finished();
}

static void vdp_data_w(uint16_t data)
{
    s.pending = 0;
    vdp_bus_w(data);
    if (s.dmafill) {
        s.dmafill = 0;
        s.dma_length = dma_length_reg();
        dma_update();
    }
}

static uint16_t vdp_data_r(void)
{
    GVDP *v = V;
    uint16_t data = 0;
    s.pending = 0;
    switch (v->code & 0x1F) {
    case 0x00: {
        uint16_t a = (uint16_t)(v->address & 0xFFFE);
        if (g_ws_window) data = (uint16_t)(ws_vram_game8(a) << 8 | ws_vram_game8((uint16_t)(a + 1)));   /* the game's VRAM, not the view's */
        else data = (uint16_t)((v->vram[a] << 8) | v->vram[(uint16_t)(a + 1)]);
        break;
    }
    case 0x04: {
        unsigned idx = (v->address & 0x7E) >> 1;
        if (idx >= 40) idx = 0;
        data = (uint16_t)((v->vsram[idx] & 0x7FF) | (s.fifo[s.fifo_idx] & 0xF800));
        break;
    }
    case 0x08:
        data = (uint16_t)((v->cram[(v->address >> 1) & 0x3F] & 0x0EEE) | (s.fifo[s.fifo_idx] & ~0x0EEE));
        break;
    case 0x0C: {
        uint8_t b = g_ws_window ? ws_vram_game8((uint16_t)(v->address ^ 1)) : v->vram[(uint16_t)(v->address ^ 1)];
        data = (uint16_t)((s.fifo[s.fifo_idx] & 0xFF00) | b);
        break;
    }
    default:
        break;
    }
    v->address = (uint16_t)(v->address + REG(15));
    return data;
}

/* A register write: the interrupt lines and the H/V latch it may move are the timed machine's only
   (the frame-synchronous one takes interrupts at its own points, and its H/V counter reads 0). */
static void vdp_reg_w(unsigned r, uint8_t d)
{
    if (!(REG(1) & 4) && r > 10) return;   /* mode 4: #11-#23 locked */
    if (r < GVDP_NUM_REGISTERS) REG(r) = d;
}

static void vdp_ctrl_w(uint16_t data)
{
    GVDP *v = V;
    if (!s.pending) {
        if (s.dma_length && s.dma_type < 2) {   /* 68K DMA still running */
            s.cached_write = data;
            return;
        }
        if ((data & 0xC000) == 0x8000) vdp_reg_w((data >> 8) & 0x1F, (uint8_t)data);
        else                           s.pending = (uint8_t)(REG(1) & 4);
        v->address = (uint16_t)(s.addr_latch | (data & 0x3FFF));
        v->code = (uint8_t)((v->code & 0x3C) | ((data >> 14) & 0x03));
        return;
    }
    s.pending = 0;
    s.addr_latch = (uint16_t)((data & 3) << 14);
    v->address = (uint16_t)(s.addr_latch | (v->address & 0x3FFF));
    v->code = (uint8_t)((v->code & 0x03) | ((data >> 2) & 0x3C));
    if ((v->code & 0x20) && (REG(1) & 0x10)) {
        switch (REG(23) >> 6) {
        case 2:                                   /* fill: waits for a data write */
            s.dma_type = 2;
            s.dmafill = 1;
            break;
        case 3:                                   /* VRAM copy */
            s.dma_type = 3;
            s.dma_length = dma_length_reg();
            s.dma_src = ((uint32_t)REG(22) << 8) | REG(21);
            dma_update();
            break;
        default:                                  /* 68K bus -> VDP */
            s.dma_type = (v->code & 0x06) ? 0 : 1;
            s.dma_length = dma_length_reg();
            s.dma_src = ((uint32_t)REG(22) << 8) | REG(21);
            dma_update();
            break;
        }
    }
}

/* The status read's effect (the frame-synchronous machine returns its own word, below) */
static void vdp_ctrl_r(void)
{
    s.pending = 0;
    g_machine.vdp.sprite_overflow = g_machine.vdp.sprite_collision = 0;
}

static void vdp_w16(uint32_t a, uint16_t v)
{
    switch (a & 0x1C) {
    case 0x00: vdp_data_w(v); return;
    case 0x04: vdp_ctrl_w(v); return;
    case 0x10: case 0x14: audio_event_push(0, AUDIO_PORT_PSG, (uint8_t)v); return;   /* PSG */
    default: return;
    }
}

/* ======================================================================== */
/* I/O (exact_sched.c) */

static uint8_t pad_read(int port)
{
    uint8_t data = (uint8_t)((s.pad_state[port] & 0x40) | 0x3F);
    uint16_t g = g_machine.bus.pad[port];
    /* GPAD layout -> the reference's: X and Z trade places in bits 8-10 */
    uint32_t val = (g & 0x0FFu) | ((g & GPAD_X) ? 0x400 : 0) | ((g & GPAD_Y) ? 0x200 : 0)
                 | ((g & GPAD_Z) ? 0x100 : 0) | ((g & GPAD_MODE) ? 0x800 : 0);
    unsigned step = (unsigned)(s.pad_counter[port] | ((data >> 6) & 1));
    int six = g_machine.bus.pad_type[port] == 1;
    if (!six) step &= 1;
    switch (step) {
    case 1: case 3: case 5:
        data &= (uint8_t)~(val & 0x3F); break;
    case 0: case 2:
        data &= (uint8_t)~(val & 0x03); data &= (uint8_t)~((val >> 2) & 0x30); data &= (uint8_t)~0x0C; break;
    case 4:
        data &= (uint8_t)~((val >> 2) & 0x30); data &= (uint8_t)~0x0F; break;
    case 6:
        data &= (uint8_t)~((val >> 2) & 0x30); break;
    case 7:
        data &= (uint8_t)~(val & 0x30); data &= (uint8_t)~((val >> 8) & 0x0F); break;
    }
    return data;
}

static void pad_write(int port, uint8_t data, uint8_t mask)
{
    data = (uint8_t)((s.pad_state[port] & ~mask) | (data & mask));
    if (g_machine.bus.pad_type[port] == 1 && !(s.pad_state[port] & 0x40) && (data & 0x40)) {
        s.pad_counter[port] = (uint8_t)((s.pad_counter[port] + 2) & 6);
        s.pad_timeout[port] = 0;
    }
    s.pad_state[port] = data;
}

static void input_refresh(void)
{
    for (int p = 0; p < 2; p++)
        if (g_machine.bus.pad_type[p] == 1 && s.pad_timeout[p]++ > 25) {
            s.pad_counter[p] = 0;
            s.pad_timeout[p] = 0;
        }
}

static uint8_t io_read(unsigned off)
{
    switch (off) {
    case 0x01: case 0x02: case 0x03: {
        uint8_t mask = (uint8_t)(0x80 | s.io_reg[off + 3]);
        /* port C and an unplugged pad port: the reference's dummy_read */
        uint8_t data = off == 3 || g_machine.bus.pad_type[off - 1] == 2 ? 0x7F : pad_read((int)off - 1);
        return (uint8_t)((s.io_reg[off] & mask) | (data & ~mask));
    }
    default:
        return s.io_reg[off];
    }
}

static void io_write(unsigned off, uint8_t data)
{
    switch (off) {
    case 0x01: case 0x02: case 0x03:
        s.io_reg[off] = data;
        if (off < 3) pad_write((int)off - 1, data, s.io_reg[off + 3]);
        return;
    case 0x04: case 0x05: case 0x06:
        if (data != s.io_reg[off]) {
            s.io_reg[off] = data;
            if (off < 6) pad_write((int)off - 4, s.io_reg[off - 3], data);
        }
        return;
    case 0x07: case 0x0A: case 0x0D:
        s.io_reg[off] = data; return;
    case 0x09: case 0x0C: case 0x0F:
        s.io_reg[off] = (uint8_t)(data & 0xF8); return;
    default:
        return;
    }
}

/* ======================================================================== */
/* The 68000's hardware accesses (exact_sched.c xbus_* and fsync_read, for the hardware ports) */

static void hang_guard(uint32_t a)
{
    if (++s.fs_reads > 20000000u) {
        fprintf(stderr, "[stepped] no sync point after %llu hardware reads: stuck polling $%06X in $%06X "
                        "(sync %llu, Z80 state %d)\n", (unsigned long long)s.fs_reads, (unsigned)a,
                (unsigned)s.last_entry, (unsigned long long)s.sync_no, s.zstate);
        fflush(stderr);
        exit(3);
    }
}

static int vdp_port(uint32_t a) { return a >= 0xC00000u && a < 0xC00020u; }

static uint16_t vdp_read(uint32_t a, int size)
{
    switch (a & 0x1C) {
    case 0x00: {
        uint16_t w = vdp_data_r();
        return size == 1 ? (uint16_t)((a & 1) ? (w & 0xFF) : (w >> 8)) : w;
    }
    case 0x04: {
        /* FIFO empty, no DMA in progress; VBLANK alternates from read to read, clear first after a
           V-int, so any wait for the bit to change ends at the next read */
        vdp_ctrl_r();
        uint16_t w = (uint16_t)(0x7200 | (s.fs_vblank ? 0x08 : 0x00) | (!(REG(1) & 0x40) ? 0x08 : 0x00));
        s.fs_vblank ^= 1;
        return size == 1 ? (uint16_t)((a & 1) ? (w & 0xFF) : (w >> 8)) : w;
    }
    default:
        return 0;   /* H/V counter: no beam without time */
    }
}

static uint8_t z80_area_read(uint32_t a)
{
    if (!s.zaccess)
        machine_fatal("a 68000 read of $%06X without the Z80's bus (open bus: not modelled)", (unsigned)a);
    if ((a & 0x6000) == 0x4000)
        machine_fatal("a 68000 read of the YM2612 ($%06X): not modelled (the game's 68000 never touches it)", (unsigned)a);
    if ((a & 0x6000) == 0x6000) return 0xFF;
    return snd_68k_read((uint16_t)(a & 0x1FFF), g_machine.bus.z80_ram[a & 0x1FFF]);   /* machine 1's (sound2.h) */
}

static void z80_area_w8(uint32_t a, uint8_t v)
{
    if (!s.zaccess) return;
    if ((a & 0x6000) == 0x0000 || (a & 0x6000) == 0x2000) {
        snd_68k_write((uint16_t)(a & 0x1FFF), v);   /* machine 2's copy, but the effect window (sound2.h) */
        g_machine.bus.z80_ram[a & 0x1FFF] = v;
        return;
    }
    if ((a & 0x6000) == 0x4000)
        machine_fatal("a 68000 write of the YM2612 ($%06X): not modelled (the game's 68000 never touches it)", (unsigned)a);
    /* the bank register and the rest of the area: nothing the bus routes there */
}

extern "C" uint8_t nm_bus_read8(uint32_t a)
{
    a &= 0xFFFFFFu;
    hang_guard(a);
    if (vdp_port(a)) return (uint8_t)vdp_read(a, 1);
    if (a >= 0xA00000u && a < 0xA10000u) return z80_area_read(a);
    if (a >= 0xA10000u && a < 0xA10020u) return io_read((a >> 1) & 0x0F);
    if (a == 0xA11100u) return (uint8_t)(s.zstate == 3 ? 0x00 : 0x01);   /* open-bus bits: 0 */
    if (a == 0xA11101u) return 0x00;
    if (a == 0xA11200u) return (uint8_t)(g_machine.bus.z80_reset_off ? 0x01 : 0x00);
    if (a == 0xA11201u) return 0x00;
    machine_fatal("a 68000 byte read of $%06X: not modelled", (unsigned)a);
}

extern "C" uint16_t nm_bus_read16(uint32_t a)
{
    a &= 0xFFFFFFu;
    hang_guard(a);
    if (vdp_port(a)) return vdp_read(a, 2);
    if (a >= 0xA00000u && a < 0xA10000u) { uint8_t b = z80_area_read(a); return (uint16_t)(b << 8 | b); }
    if (a >= 0xA10000u && a < 0xA10020u) { uint8_t b = io_read((a >> 1) & 0x0F); return (uint16_t)(b << 8 | b); }
    if (a == 0xA11100u) return (uint16_t)(s.zstate == 3 ? 0x0000 : 0x0100);   /* open-bus bits: 0 */
    if (a == 0xA11200u) return (uint16_t)(g_machine.bus.z80_reset_off ? 0x0100 : 0x0000);
    machine_fatal("a 68000 word read of $%06X: not modelled", (unsigned)a);
}

extern "C" void nm_bus_write16(uint32_t a, uint16_t v)
{
    a &= 0xFFFFFFu;
    if (vdp_port(a)) { vdp_w16(a, v); return; }
    if (a >= 0xA00000u && a < 0xA10000u) { z80_area_w8(a, (uint8_t)(v >> 8)); return; }
    if (a >= 0xA10000u && a < 0xA10020u) { io_write((a >> 1) & 0x0F, (uint8_t)v); return; }
    if (a == 0xA11100u) { z80_busreq_w((v >> 8) & 1); return; }
    if (a == 0xA11200u) { z80_reset_w((v >> 8) & 1); return; }
    if (a >= 0xA14000u && a < 0xA14004u) return;   /* TMSS: nothing to unlock here */
    machine_fatal("a 68000 word write of $%06X: not modelled", (unsigned)a);
}

extern "C" void nm_bus_write8(uint32_t a, uint8_t v)
{
    a &= 0xFFFFFFu;
    if (a >= 0xA00000u && a < 0xA10000u) { z80_area_w8(a, v); return; }
    if (a >= 0xA10000u && a < 0xA10020u) { if (a & 1) io_write((a >> 1) & 0x0F, v); return; }
    if (a == 0xA11100u) { z80_busreq_w(v & 1); return; }
    if (a == 0xA11200u) { z80_reset_w(v & 1); return; }
    /* the odd bytes of the bus-control registers: the engine's plain bus sees them (its flags only) */
    if (a == 0xA11101u) { g_machine.bus.z80_busreq = (uint8_t)(v & 1); return; }
    if (a == 0xA11201u) { g_machine.bus.z80_reset_off = (uint8_t)(v & 1); return; }
    if (vdp_port(a)) {
        if ((a & 0x1C) == 0x10 || (a & 0x1C) == 0x14) { audio_event_push(0, AUDIO_PORT_PSG, v); return; }
        vdp_w16(a & ~1u, (uint16_t)(v << 8 | v));
        return;
    }
    if (a >= 0xA14000u && a < 0xA14004u) return;   /* TMSS */
    machine_fatal("a 68000 byte write of $%06X: not modelled", (unsigned)a);
}

/* ======================================================================== */
/* Interrupts: the machine decides when; the flow runtime does the 68000's side (the exception, the
   vector's handler in C, its RTE back to where it came: dk::flow::interrupt) */

static void irq_ack(void)
{
    /* the VDP clears the acknowledged latch; V-int has priority */
    if (s.vint_pending & REG(1)) s.vint_pending = 0;
    else s.hint_pending = 0;
}

static unsigned sr_mask(void) { return (dk_native_sr() >> 8) & 7u; }

static void take_interrupt(uint32_t pc, unsigned level)
{
    irq_ack();
    s.depth++;
    dk_native_interrupt(level, pc);
    s.depth--;
}

static int fsync_hint(uint32_t pc)
{
    if (!(s.hint_pending & REG(0) & 0x10) || sr_mask() >= 4u) return 0;
    take_interrupt(pc, 4);
    return 1;
}

/* The display after a V-int, line by line with the H-int raster (ruling R3; exact_sched.c
   fsync_display with GameSpec.fsync_raster). */
static void vdp_frame_counters(void);

static void fsync_display(uint32_t pc)
{
    int h = (REG(1) & 0x04) ? ((REG(1) & 0x08) ? 240 : 224) : 192;
    int counter = REG(10);
    const int hf = s_render && g_opt.high_fps;          /* phase 6 R3: present.cpp's capture */
    if (s_render) { gvdp_reset_stats(); ws_frame_setup(); }   /* phase 6: the 16:9 view (widescreen.cpp), captured with it */
    if (hf) hf_display_begin();
    fsync_hint(pc);                                     /* a latch the V-int re-enabled */
    for (int line = 0; line <= h; line++) {             /* the active lines, then vblank's first */
        if (--counter < 0) { counter = REG(10); s.hint_pending = 0x10; }
        if (line < h && s_render) {
            ws_line(line);
            if (hf) hf_display_line(line);
            render_line(line);
        }
        fsync_hint(pc);
    }
    if (hf) hf_display_end();
    if (s_render) { ws_frame_check(); vdp_frame_counters(); }
}

/* Phase 6 (R10, R12): the VDP model's counters for one drawn frame into the enhance counters
   (enhance.h). The model fills the per-line drops and masking (sprites) and the margin columns
   still at the edge clamp (ws); the groups' `active` belongs to the pieces that complete them
   (the game's display-list counts, the widescreen code). Rendered frames only. */
static void vdp_frame_counters(void)
{
    GVDPStats st;
    gvdp_get_stats(&st);
    uint64_t drops = st.sprite_drops_count + st.sprite_drops_pixels;
    g_enh.sprites.line_drops += drops;
    if (drops) g_enh.sprites.line_drop_frames++;
    g_enh.sprites.x0_masked_lines += st.sprite_masked_lines;
    g_enh.ws.edge_clamped_columns += st.ws_edge_clamped_columns;
}

/* The game's fiber: it runs from the reset to each sync point and parks there. */
static fiber_t s_host_fiber, s_game_fiber;
static int s_game_done;

static void park(void) { fiber_switch(s_host_fiber); }

/* Game time for the watchdog (runner.cpp): every sync point and every IRQ_WAIT V-int. Not part of
   the snapshot: it only ever counts up, so a restore is never mistaken for no progress. */
static volatile uint64_t s_progress;
extern "C" uint64_t nm_progress(void) { return s_progress; }

/* The longest sync interval a run can have: no more V-ints than its input has reference frames
   left (the reference has no frame past them). Without an input script, one hour of game time. */
static void interval_bound(uint32_t j, uint32_t n)
{
    uint64_t limit = ns_active() ? (uint64_t)n + ns_peek_frames() : 216000u;
    if (j < limit) return;
    char why[256];
    snprintf(why, sizeof why, "sync interval after sync %llu took V-int %u, past the %llu reference frames its "
                              "input has (%u of them VPAD)", (unsigned long long)s.sync_no, j + 1,
             (unsigned long long)limit, n);
    nm_stuck(why);
}

/* IRQ_WAIT (spec.c wait_vint_here + exact_sched.c x68k_wait_vint, GameSpec.fsync_vint_at_wait): the
   pads of the interval's later frames for each V-int after its first, then the V-int (enabled and
   unmasked), latched as the timed one so its acknowledge leaves an H-int latch alone, then the
   display with its raster. The interrupted PC is 0 (a C wait has none; the RTE returns there). */
extern "C" void nm_wait_vint(void)
{
    s_progress = s_progress + 1;
    if (s.vints) {
        if (s_play) {
            /* played: a further V-int of the interval is the next displayed frame: the host
               presents, takes the controllers' pads and runs the Z80's frame first */
            s_extra = 1;
            park();
        } else {
            uint32_t n = ns_vpad_count(), j = s.vints - 1;
            if (j >= n) interval_bound(j, n);
            g_machine.bus.pad[0] = j < n ? ns_vpad(j, 0) : ns_peek(j - n, 0);
            g_machine.bus.pad[1] = j < n ? ns_vpad(j, 1) : ns_peek(j - n, 1);
        }
    }
    s.vints++;
    if ((REG(1) & 0x20) && sr_mask() < 6u) {
        s.vint_pending = 0x20;
        take_interrupt(0u, 6);
        s.fs_vblank = 0;
        fsync_display(0u);
    }
}

/* A game function entered: at depth 0, DK_SYNC_PC is the sync point (its row, then the runner
   frame ends: the fiber parks until the host runs the next). */
extern "C" void nm_entry(uint32_t addr)
{
    s.last_entry = addr;
    if (s.depth || addr != DK_SYNC_PC) return;
    s.sync_no++;
    s_progress = s_progress + 1;
    nm_on_sync(s.sync_no, s.frame_no);
    /* the watchdog's control (NOTES "The native watchdog"): BR_PLANT_HANG=S spins the game thread
       for ever at sync point S, with no V-int and no sync point, as a game stuck on a RAM flag */
    static long long plant = -2;
    if (plant == -2) {
        const char *e = getenv("BR_PLANT_HANG");
        plant = e && *e ? strtoll(e, nullptr, 0) : -1;
    }
    if ((long long)s.sync_no == plant)
        for (;;) *(volatile uint32_t *)&s.last_entry = addr;
    s.vints = 0;   /* a new sync interval: its first V-int reads the runner frame's pads */
    s_extra = 0;
    park();
    s.fs_reads = 0;
}

static void game_main(void *arg)
{
    (void)arg;
    nm_game_start();
    dk_native_start(0x000300u);   /* the ROM's reset vector: Boot_Reset; never returns */
    s_game_done = 1;
    for (;;) park();
}

#define GAME_STACK ((size_t)32u * 1024u * 1024u)

/* ======================================================================== */
/* The frame */

/* The Z80's frame (exact_sched.c fsync_z80_frame): its V-int line for one line from where it is, the
   rest of the frame, the YM sample clock re-anchored; the interval's bus releases then run on the
   re-based clock (audio stamped one frame on). */
static void z80_frame(void)
{
    const uint32_t F = LINES * MPL;
    snd_frame_begin();
    s.stamp_base = 0;
    for (int i = 0; i < 262; i++) input_refresh();
    if (s.zstate == 1) {
        z80_irq(1);
        z80_step(&s.zclk, s.zclk + MPL);
        z80_irq(0);
    }
    if (s.zstate == 1 && s.zclk < F) z80_step(&s.zclk, F);
    if (s.zclk < F) s.zclk = F;
    fm_update(F);
    {
        uint32_t span = F > s.fm_start ? F - s.fm_start : 0;
        uint32_t k = (span + FM_SAMPLE - 1) / FM_SAMPLE;
        if (k == 0) k = 1;
        s.fm_count = s.fm_start = s.fm_start + k * FM_SAMPLE - F;
    }
    s.zclk -= F;
    s.stamp_base = F;
    snd_frame_end();   /* machine 2's frame (music_voices) */
}

extern "C" void nm_run_frame(void)
{
    s.frame_no++;
    z80_frame();
    s_extra = 0;
    if (!s_game_done) fiber_switch(s_game_fiber);
    g_snd_frame++;
}

extern "C" int nm_extra_frame(void) { return s_extra; }
extern "C" int nm_game_stopped(void) { return s_game_done; }
extern "C" void nm_set_render(int on) { s_render = on; }
extern "C" void nm_set_play(int on) { s_play = on; }
extern "C" uint64_t nm_sync_count(void) { return *(volatile uint64_t *)&s.sync_no; }
extern "C" uint32_t nm_last_entry(void) { return *(volatile uint32_t *)&s.last_entry; }

extern "C" void nm_set_pads(uint16_t p1, uint16_t p2)
{
    g_machine.bus.pad[0] = p1;
    g_machine.bus.pad[1] = p2;
}

extern "C" void nm_set_pad_types(int t1, int t2)
{
    g_machine.bus.pad_type[0] = (uint8_t)(t1 == 1 || t1 == 2 ? t1 : 0);
    g_machine.bus.pad_type[1] = (uint8_t)(t2 == 1 || t2 == 2 ? t2 : 0);
}

/* ======================================================================== */
/* Power-on (genesis_machine.c machine_init + exact_sched.c exact_reset) */

static uint16_t vdp_bus_read(void *u, uint32_t a)   /* the model's own DMA path: unused here (DMA is above) */
{
    (void)u;
    return (uint16_t)((bus_peek8(a, "the video model's DMA") << 8) | bus_peek8(a + 1u, "the video model's DMA"));
}

static void wire_pointers(void)
{
    g_machine.vdp.bus_read = vdp_bus_read;
    g_machine.vdp.bus_read_user = nullptr;
    g_machine.vdp.colour_updated = colour_cb;
    g_machine.vdp.colour_updated_user = nullptr;
    g_machine.bus.vdp = &g_machine.vdp;
    g_machine.z80.read_byte = z80_read;
    g_machine.z80.write_byte = z80_write;
    g_machine.z80.port_in = z80_port_in;
    g_machine.z80.port_out = z80_port_out;
    g_machine.z80.userdata = &g_machine.bus;
}

extern "C" void nm_power_on(void)
{
    memset(&g_machine, 0, sizeof g_machine);
    gvdp_init(&g_machine.vdp);
    /* R10: the VDP's per-line sprite limits lifted (the game's 80-entry cap is its native branch's,
       the sprites past it go to gvdp_set_host_sat). With a widescreen margin the line's sprite pixel
       budget stays 320 (no effect without one): the presentation mode's centre is the 4:3 line. */
    gvdp_set_unlimited_sprites(g_opt.unlimited_sprites);
    gvdp_set_ws_authentic_sprite_budget(1);
    spr_host_power_on();   /* the game's native list branches: their host side (spr_host.h) */
    ws_draw_power_on();    /* R11: the widescreen margin draws (ws_draw.h), the host sprite layer */
    g_machine.bus.version = 0x80;
    z80_init(&g_machine.z80);
    z80_own_reset(&g_machine.z80);
    wire_pointers();
    s_cram_argb[GVDP_WS_BAR_INDEX] = 0xFF000000u;

    nm_ram_init();
    ws_power_on();
    memset(&s, 0, sizeof s);
    s.cached_write = -1;
    s.io_reg[0x00] = 0xA0;   /* overseas NTSC, no expansion unit */
    s.io_reg[0x07] = 0xFF; s.io_reg[0x0A] = 0xFF; s.io_reg[0x0D] = 0xFB;
    s.pad_state[0] = s.pad_state[1] = 0x40;
    s.fm_tal = 1024; s.fm_tbl = 256 << 4; s.fm_mode = 0x30;
    z80_power_on();
    g_machine.bus.z80_busreq = 0;
    g_machine.bus.z80_reset_off = 0;
    memset(g_machine.vdp.reg, 0, sizeof g_machine.vdp.reg);
    g_machine.vdp.code = 0;
    g_machine.vdp.address = 0;
    snd_power_on();   /* machine 2 (music_voices) as machine 1 is now, the synthesis */

    s_host_fiber = fiber_convert_thread();
    s_game_fiber = fiber_create(GAME_STACK, GAME_STACK, game_main, nullptr);
    if (!s_host_fiber || !s_game_fiber) machine_fatal("cannot make the game's fiber");
}

/* ======================================================================== */
/* Branch snapshots: the game's fiber (its stack holds the C frames and the flow runtime's run
   loops), work RAM, the video model, the Z80, the machine's state, the flow runtime's state */

static void put(uint8_t *&o, const void *p, size_t n) { memcpy(o, p, n); o += n; }
static void get(const uint8_t *&i, void *p, size_t n) { memcpy(p, i, n); i += n; }

extern "C" size_t nm_snapshot_save(uint8_t **buf)
{
    size_t fb = fiber_snapshot_bound(s_game_fiber), rb = nm_ram_save(nullptr, 0), flb = dk_native_flow_save(nullptr, 0);
    size_t sb = snd_save(nullptr, 0);
    size_t need = 4 * sizeof(size_t) + fb + rb + flb + sizeof g_machine + sizeof s + sizeof g_snd_frame +
                  spr_host_save(nullptr, 0) + ws_draw_save(nullptr, 0) + ws_saved_size() + sb;
    uint8_t *b = (uint8_t *)malloc(need);
    if (!b) machine_fatal("branch snapshot: out of memory (%zu bytes)", need);
    uint8_t *o = b + 4 * sizeof(size_t);
    size_t fw = fiber_snapshot_save(s_game_fiber, o, fb);
    if (!fw) machine_fatal("branch snapshot: the game's fiber cannot be saved");
    o += fw;
    size_t rw = nm_ram_save(o, rb);
    o += rw;
    size_t flw = dk_native_flow_save(o, flb);
    o += flw;
    GenesisMachine m = g_machine;
    m.vdp.bus_read = nullptr; m.vdp.colour_updated = nullptr; m.bus.vdp = nullptr;
    m.z80.read_byte = nullptr; m.z80.write_byte = nullptr; m.z80.port_in = nullptr; m.z80.port_out = nullptr;
    m.z80.userdata = nullptr;
    put(o, &m, sizeof m);
    put(o, &s, sizeof s);
    put(o, &g_snd_frame, sizeof g_snd_frame);
    o += spr_host_save(o, spr_host_save(nullptr, 0));   /* the sync point is inside the list's frame */
    o += ws_draw_save(o, ws_draw_save(nullptr, 0));     /* R11: the margin draws, likewise */
    ws_saved_get(o);   /* phase 6: what the widescreen code was told (widescreen.h) */
    o += ws_saved_size();
    o += snd_save(o, sb);   /* machine 2 and the synthesis' views (sound2.cpp): last, snd_load takes the rest */
    size_t hdr[4] = { fw, rw, flw, (size_t)(o - b) };
    memcpy(b, hdr, sizeof hdr);
    *buf = b;
    return (size_t)(o - b);
}

extern "C" int nm_snapshot_load(const uint8_t *b, size_t len)
{
    size_t hdr[4];
    if (len < sizeof hdr) return 0;
    memcpy(hdr, b, sizeof hdr);
    if (hdr[3] != len) return 0;
    const uint8_t *i = b + sizeof hdr;
    if (fiber_snapshot_load(s_game_fiber, i, hdr[0]) != 0) return 0;
    i += hdr[0];
    if (!nm_ram_load(i, hdr[1])) return 0;
    i += hdr[1];
    dk_native_flow_load(i);
    i += hdr[2];
    get(i, &g_machine, sizeof g_machine);
    wire_pointers();
    get(i, &s, sizeof s);
    get(i, &g_snd_frame, sizeof g_snd_frame);
    spr_host_load(i);
    i += spr_host_save(nullptr, 0);
    ws_draw_load(i);
    i += ws_draw_save(nullptr, 0);
    ws_saved_put(i);
    i += ws_saved_size();
    if (!snd_load(i, len - (size_t)(i - b))) return 0;
    for (unsigned c = 0; c < GVDP_CRAM_ENTRIES; c++) colour_cb(nullptr, c, g_machine.vdp.cram[c]);
    hf_invalidate();   /* high_fps: the restored picture has no previous frame */
    audio_event_queue_reset();
    return 1;
}
