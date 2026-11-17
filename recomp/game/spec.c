/*
 * spec.c - GameSpec for The Adventures of Batman and Robin (Sega Genesis, SHA-1 1da4b3fc92a6572e70d748ebad346aeb291f89f3).
 * Written by `dkit genesis recomp init` (from xmen2's spec, which made a recomp equal to its
 * reference emulator every frame of a 76-run corpus); yours to extend.
 *
 * Boot facts (the ROM's vector table): reset SSP $00000000, entry $000300, IRQ6 (V-int)
 * $007736, IRQ4 (H-int) $FFE560. A vector into RAM runs as an engine-generated RAM code overlay
 * (dkit.toml [recomp] ram_code; the untimed stepped builds too since engine fsync-raster), and
 * only RAM code that matches no overlay on the interpreter (a nonzero interp_instructions
 * count, phase 2 K7).
 *
 * Phase-2 trace: with DKIT_TRACE_CSV=<path>, on_frame_post appends one row per completed frame,
 *   frame,ram_crc32,vram_crc32,cram_crc32,vsram_crc32,z80_crc32
 * with the reference's definitions (dkit genesis emu harness): RAM and VRAM in 68000 byte order,
 * CRAM 64 bus-format words big-endian (& $0EEE), VSRAM 40 words & $07FF big-endian, Z80 RAM 8 KB.
 * At exit it prints `[dkit] counters {...}`: the runner's own dispatch misses and interpreted
 * instructions (K7). DKIT_DUMP_FRAMES="4,5" + DKIT_DUMP_DIR write fNNNNNN.{ram,vram,cram,vsram,
 * z80,regs.json} after those frames, in the trace's layouts (first divergence: dump the
 * reference's state at the same frame and diff the two byte for byte). Under --branch-plan the
 * trace also carries the branch markers described above g_game_spec.
 *
 * Phase 4 (sync points, the stepped runners): see "sync points" below. dkit_sync.h is generated
 * by `dkit genesis recomp sync` from dkit.toml [recomp.sync].
 *
 * Phase 5 (docs/phases/P5-port.md): the native runner does not build from this spec since P6: it is
 * the native machine of recomp/platform/native (its own frame-synchronous machine, its rows from the
 * exporter's image, the user's ROM checked by SHA-1). The "exported" counter here stays null.
 */
#include "game_spec.h"
#include "genesis_runtime.h"
#include "video/genesis_machine.h"
#include "dkit_sync.h"

/* The frame-synchronous machine's H-int raster (GameSpec.fsync_raster): on for this game; a build
 * with -DDK_FSYNC_RASTER=0 is the old machine (the negative control: its sync rows are the rows
 * from before the raster). */
#ifndef DK_FSYNC_RASTER
#define DK_FSYNC_RASTER 1
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GAME_ENTRY 0x000300u
#define GAME_VINT  0x007736u
#define GAME_HINT  0xFFE560u

/* runtime/crash_win.cpp: a host exception prints where and the stack before the runner dies */
void dk_install_crash_trace(void);
void dk_crash_test(uint64_t frame);

static void game_start(void);
static void game_call_entry_point(void) { game_start(); recomp_call_addr(GAME_ENTRY); }
static void game_call_vblank(void)      { recomp_call_addr(GAME_VINT); }
static void game_call_hblank(void)      { recomp_call_addr(GAME_HINT); }

/* ---- per-frame state CRCs ---------------------------------------------- */

static uint32_t s_crc_table[256];

static void crc_init(void)
{
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t c = i;
        for (int k = 0; k < 8; k++) c = (c & 1u) ? 0xEDB88320u ^ (c >> 1) : c >> 1;
        s_crc_table[i] = c;
    }
}

static uint32_t crc32_update(uint32_t c, const uint8_t *p, size_t n)
{
    while (n--) c = s_crc_table[(c ^ *p++) & 0xFFu] ^ (c >> 8);
    return c;
}

static uint32_t crc32_bytes(const uint8_t *p, size_t n)
{
    return crc32_update(0xFFFFFFFFu, p, n) ^ 0xFFFFFFFFu;
}

/* n words (at most 64), each & mask, big-endian: the reference's CRAM / VSRAM format. */
static void words_be(uint8_t *out, const uint16_t *w, size_t n, uint16_t mask)
{
    for (size_t i = 0; i < n; i++) {
        uint16_t v = (uint16_t)(w[i] & mask);
        out[2 * i] = (uint8_t)(v >> 8);
        out[2 * i + 1] = (uint8_t)v;
    }
}

static uint32_t crc32_words_be(const uint16_t *w, size_t n, uint16_t mask)
{
    uint8_t buf[2 * 64];
    words_be(buf, w, n, mask);
    return crc32_bytes(buf, 2 * n);
}

static FILE *s_trace, *s_sync;
static uint64_t s_last_frame, s_saved_frame;   /* the last frame posted; the branch snapshot's frame */
static uint32_t s_branch_a7min = 0xFFFFFFFFu;  /* the lowest A7 of the branches played (A7 - 1 order) */
static uint64_t s_sync_rows;                    /* sync points so far (phase 4) */
static uint32_t s_stack_low = DK_STACK_LOW;
uint32_t dk_stack_low(void) { return s_stack_low; }   /* also verify.cpp's dead-stack bound */
#ifdef DK_VERIFY
int  dk_verify_entry(uint32_t addr);                   /* game/runtime/verify.cpp */
void dk_verify_branch_save(void);                      /* its testing state at a branch point */
void dk_verify_branch_restore(void);
void dk_verify_branch_end(const char *tag);            /* a branch's results */
#endif
#if defined(DK_HANDC)
void dk_handc_branch_save(void);                       /* game/runtime/handc_rt.cpp: the C frames' */
void dk_handc_branch_restore(void);                    /* state, beside the snapshot's fiber stack */
#endif

/* The runner's counters (K7): raw dispatch-miss events (every computed jump/call with no
 * generated function), every instruction the interpreter retired (RAM code, a miss's fallback).
 * This spec installs no dispatch_override, the only hook that can stand in for generated code,
 * so stand-in hits are structurally 0. */
extern uint64_t glue_miss_count_any(void);
extern uint64_t g_m68ki_insn_total;

static void game_report_counters(void)
{
    if (s_trace) fflush(s_trace);
    if (s_sync) fflush(s_sync);
    /* a7_min: the lowest A7 this run saw (timed code only; null when untimed): D1 measures
     * [recomp.sync] stack_low as the corpus's lowest */
    char a7[16] = "null";
    if (g_xa7min != 0xFFFFFFFFu) snprintf(a7, sizeof a7, "%u", (unsigned)(g_xa7min & 0xFFFFFFu));
    char exported[640] = "null";   /* the native runner's exporter counts (runtime/export.cpp) */
    fprintf(stderr, "[dkit] counters {\"dispatch_misses\": %llu, \"interp_instructions\": %llu, "
                    "\"stub_hits\": 0, \"sync_points\": %llu, \"a7_min\": %s, \"stack_low\": %u, \"exported\": %s}\n",
            (unsigned long long)glue_miss_count_any(), (unsigned long long)g_m68ki_insn_total,
            (unsigned long long)s_sync_rows, a7, (unsigned)(s_stack_low & 0xFFFFFFu), exported);
    fflush(stderr);
}

/* ---- instruction coverage (the stepped runners; D4 counts stepped-gen's where D2 is equal) --
 * DKIT_COVERAGE=<dir>: every ROM instruction the untimed generated code starts is marked
 * (genesis_runtime.h XU / g_xcov). A branch plan's branch writes <dir>/<TAG>.txt when it ends:
 * the whole run's coverage (the carrier's prefix up to the branch point, then the branch); the
 * carrier's map is kept at each branch point and put back after each branch. At exit the
 * process's own run (the carrier) writes <dir>/_own.txt. Format: one hex pc per line. Hand C
 * marks nothing (it has no XU), so only the stepped-gen runner's maps say what ran. */
#if defined(DK_STEPPED) && defined(XCOV_LIMIT)
static uint8_t *s_cov_saved;
static char s_cov_dir[400];

static void cov_write(const char *name)
{
    if (!g_xcov) return;
    char path[512];
    snprintf(path, sizeof path, "%s/%s.txt", s_cov_dir, name);
    FILE *f = fopen(path, "w");
    if (!f) { fprintf(stderr, "[dkit] coverage: cannot write %s\n", path); return; }
    setvbuf(f, NULL, _IOFBF, 1 << 20);
    for (uint32_t i = 0; i < XCOV_BYTES; i++)
        if (g_xcov[i]) fprintf(f, "%06X\n", (unsigned)(i << 1));
    fclose(f);
}

static void cov_at_exit(void) { cov_write("_own"); }

static void cov_init(void)
{
    const char *d = getenv("DKIT_COVERAGE");
    if (!d || !*d) return;
    snprintf(s_cov_dir, sizeof s_cov_dir, "%s", d);
    g_xcov = (uint8_t *)calloc(XCOV_BYTES, 1);
    s_cov_saved = (uint8_t *)calloc(XCOV_BYTES, 1);
    if (!g_xcov || !s_cov_saved) { fprintf(stderr, "[dkit] coverage: out of memory\n"); exit(3); }
    atexit(cov_at_exit);
}
static void cov_branch_save(void)    { if (g_xcov) memcpy(s_cov_saved, g_xcov, XCOV_BYTES); }
static void cov_branch_restore(void) { if (g_xcov) memcpy(g_xcov, s_cov_saved, XCOV_BYTES); }
#else
static void cov_init(void) {}
static void cov_write(const char *name) { (void)name; }
static void cov_branch_save(void) {}
static void cov_branch_restore(void) {}
#endif

static void sync_init(void);
#ifdef DK_STEPPED
static void watchdog_start(void);
#endif


/* Before the game's first instruction (call_entry_point, on the game fiber). */
static void game_start(void)
{
    dk_install_crash_trace();
    crc_init();
    atexit(game_report_counters);
    cov_init();
    sync_init();
#ifdef DK_STEPPED
    watchdog_start();
#endif
}

/* The runner has no post-reset call site for game hooks; this runs on the first frame. */
static void trace_init(void)
{
    const char *path = getenv("DKIT_TRACE_CSV");
    if (!path || !path[0]) return;
    crc_init();
    s_trace = fopen(path, "w");
    if (!s_trace) {
        fprintf(stderr, "[dkit] cannot open DKIT_TRACE_CSV %s\n", path);
        exit(2);
    }
    setvbuf(s_trace, NULL, _IOFBF, 1 << 20);
    fprintf(s_trace, "frame,ram_crc32,vram_crc32,cram_crc32,vsram_crc32,z80_crc32\n");
}

static void dump_file(const char *dir, uint64_t frame, const char *ext, const void *p, size_t n)
{
    char path[600];
    snprintf(path, sizeof path, "%s/f%06llu.%s", dir, (unsigned long long)frame, ext);
    FILE *f = fopen(path, "wb");
    if (!f) { fprintf(stderr, "[dkit] cannot write %s\n", path); return; }
    fwrite(p, 1, n, f);
    fclose(f);
}

static void dump_words(const char *dir, uint64_t frame, const char *ext, const uint16_t *w, size_t n, uint16_t mask)
{
    uint8_t buf[2 * 64];
    words_be(buf, w, n, mask);
    dump_file(dir, frame, ext, buf, 2 * n);
}

static void maybe_dump(uint64_t frame)
{
    const char *list = getenv("DKIT_DUMP_FRAMES"), *dir = getenv("DKIT_DUMP_DIR");
    if (!list || !dir) return;
    for (const char *s = list; *s; ) {
        char *e;
        unsigned long long f = strtoull(s, &e, 10);
        if (e == s) break;
        if (f == frame) {
            const GVDP *v = &g_machine.vdp;
            dump_file(dir, frame, "ram", g_ram, 0x10000u);
            dump_file(dir, frame, "vram", v->vram, GVDP_VRAM_SIZE);
            dump_words(dir, frame, "cram", v->cram, GVDP_CRAM_ENTRIES, 0x0EEE);
            dump_words(dir, frame, "vsram", v->vsram, GVDP_VSRAM_ENTRIES, 0x07FF);
            dump_file(dir, frame, "z80", g_machine.bus.z80_ram, sizeof g_machine.bus.z80_ram);
            char path[600], js[700];
            int n = 0;
            snprintf(path, sizeof path, "%s/f%06llu.regs.json", dir, (unsigned long long)frame);
            n += snprintf(js + n, sizeof js - n, "{");
            for (int i = 0; i < 8; i++) n += snprintf(js + n, sizeof js - n, "\"d%d\": \"%08X\", ", i, g_cpu.D[i]);
            for (int i = 0; i < 8; i++) n += snprintf(js + n, sizeof js - n, "\"a%d\": \"%08X\", ", i, g_cpu.A[i]);
            snprintf(js + n, sizeof js - n, "\"pc\": \"%08X\", \"sr\": \"%08X\"}", g_cpu.PC, (unsigned)g_cpu.SR);
            FILE *fj = fopen(path, "w");
            if (fj) { fputs(js, fj); fclose(fj); }
        }
        s = *e ? e + 1 : e;
    }
}

static void game_on_frame_post(uint64_t frame)
{
    static int started;
    if (!started) { started = 1; trace_init(); }
    dk_crash_test(frame);
    maybe_dump(frame);
    s_last_frame = frame;
    if (!s_trace) return;
    const GVDP *v = &g_machine.vdp;
    fprintf(s_trace, "%llu,%08x,%08x,%08x,%08x,%08x\n", (unsigned long long)frame,
            crc32_bytes(g_ram, 0x10000u),
            crc32_bytes(v->vram, GVDP_VRAM_SIZE),
            crc32_words_be(v->cram, GVDP_CRAM_ENTRIES, 0x0EEE),
            crc32_words_be(v->vsram, GVDP_VSRAM_ENTRIES, 0x07FF),
            crc32_bytes(g_machine.bus.z80_ram, sizeof g_machine.bus.z80_ram));
}

/* ---- sync points (phase 4: docs/phases/P4-decomp.md) ------------------------------------
 * A main-program entry to DK_SYNC_PC (dkit.toml [recomp.sync] pc) is a sync point: the logic of
 * a game frame is done. With DKIT_SYNC_CSV=<path> each one appends
 *   sync,frame,ram_crc32,vram_crc32,cram_crc32,vsram_crc32,z80_crc32,scene
 * (scene: the RAM long at [recomp.sync] scene, reported only) with the frame rows' definitions, except that RAM in [stack_low, stack top) is zeroed: the
 * whole 68000 stack area, from the lowest A7 the timed runner sees over the corpus to the reset
 * SSP (its bytes depend on where interrupts landed, above A7 too, and hand-written C keeps no
 * 68000 frames; xmen2 P4 decision 6). DKIT_STACK_LOW overrides stack_low (measuring). The
 * timed runner writes them beside its frame rows (D1); the frame-synchronous stepped runners
 * write only these (D2, D6). DKIT_SYNC_DUMP="661,662" + DKIT_DUMP_DIR write sNNNNNN.{ram,vram}
 * at those sync points (RAM masked like the CRC). */

/* Masked RAM: the dead stack [stack_low, top) and [recomp] dont_care (dkit_sync.h: bytes nothing
 * reads, a ruling) count as zeros. s_dc_map is dont_care alone: verify.cpp masks it too, with its
 * own dead-stack bound. Built at the first use, after sync_init has read DKIT_STACK_LOW. */
typedef struct RamSeg { uint32_t lo, hi; int masked; } RamSeg;
static uint8_t s_crc_mask[0x10000], s_dc_map[0x10000];
static RamSeg s_segs[2 * DK_DONT_CARE + 4];
static int s_nsegs = -1;

static void ram_mask_build(void)
{
    uint32_t lo = s_stack_low & 0xFFFFu, hi = (DK_STACK_TOP - 1u) & 0xFFFFu;
    memset(s_crc_mask, 0, sizeof s_crc_mask);
    memset(s_dc_map, 0, sizeof s_dc_map);
    if ((s_stack_low & 0xFF0000u) == 0xFF0000u && hi >= lo)   /* stack_low not measured: no stack mask */
        memset(s_crc_mask + lo, 1, hi + 1u - lo);
    for (int i = 0; i < DK_DONT_CARE; i++) {
        memset(s_dc_map + dk_dont_care[i].lo, 1, dk_dont_care[i].hi - dk_dont_care[i].lo);
        memset(s_crc_mask + dk_dont_care[i].lo, 1, dk_dont_care[i].hi - dk_dont_care[i].lo);
    }
    s_nsegs = 0;
    for (uint32_t a = 0; a < 0x10000u; ) {
        uint32_t b = a;
        while (b < 0x10000u && s_crc_mask[b] == s_crc_mask[a]) b++;
        s_segs[s_nsegs].lo = a;
        s_segs[s_nsegs].hi = b;
        s_segs[s_nsegs++].masked = s_crc_mask[a];
        a = b;
    }
}

const uint8_t *dk_dont_care_map(void)   /* verify.cpp: 1 = a dont_care byte */
{
    if (s_nsegs < 0) ram_mask_build();
    return s_dc_map;
}

static uint32_t ram_crc_masked(const uint8_t *ram)
{
    static const uint8_t zeros[256];
    if (s_nsegs < 0) ram_mask_build();
    uint32_t c = 0xFFFFFFFFu;
    for (int i = 0; i < s_nsegs; i++) {
        uint32_t n = s_segs[i].hi - s_segs[i].lo;
        if (!s_segs[i].masked) {
            c = crc32_update(c, ram + s_segs[i].lo, n);
            continue;
        }
        while (n) {
            uint32_t k = n > sizeof zeros ? (uint32_t)sizeof zeros : n;
            c = crc32_update(c, zeros, k);
            n -= k;
        }
    }
    return c ^ 0xFFFFFFFFu;
}

static void maybe_dump_sync(uint64_t sync, const uint8_t *work_ram, const uint8_t *vram)
{
    const char *list = getenv("DKIT_SYNC_DUMP"), *dir = getenv("DKIT_DUMP_DIR");
    if (!list || !dir) return;
    for (const char *s = list; *s; ) {
        char *e;
        unsigned long long n = strtoull(s, &e, 10);
        if (e == s) break;
        if (n == sync) {
            static uint8_t ram[0x10000];
            if (s_nsegs < 0) ram_mask_build();
            for (uint32_t i = 0; i < sizeof ram; i++) ram[i] = s_crc_mask[i] ? 0 : work_ram[i];
            char path[600];
            snprintf(path, sizeof path, "%s/s%06llu.ram", dir, n);
            FILE *f = fopen(path, "wb");
            if (f) { fwrite(ram, 1, sizeof ram, f); fclose(f); }
            snprintf(path, sizeof path, "%s/s%06llu.vram", dir, n);
            f = fopen(path, "wb");
            if (f) { fwrite(vram, 1, GVDP_VRAM_SIZE, f); fclose(f); }
        }
        s = *e ? e + 1 : e;
    }
}

#ifdef DK_STEPPED
static uint32_t s_vints;
#endif
static void game_on_sync(uint64_t sync, uint64_t frame)
{
    s_sync_rows = sync;
#ifdef DK_STEPPED
    s_vints = 0;   /* a new sync interval: its first V-int reads the runner frame's pads */
#endif
    maybe_dump_sync(sync, g_ram, g_machine.vdp.vram);
    if (!s_sync) return;
    const GVDP *v = &g_machine.vdp;
    fprintf(s_sync, "%llu,%llu,%08x,%08x,%08x,%08x,%08x,%08x\n", (unsigned long long)sync, (unsigned long long)frame,
            ram_crc_masked(g_ram),
            crc32_bytes(v->vram, GVDP_VRAM_SIZE),
            crc32_words_be(v->cram, GVDP_CRAM_ENTRIES, 0x0EEE),
            crc32_words_be(v->vsram, GVDP_VSRAM_ENTRIES, 0x07FF),
            crc32_bytes(g_machine.bus.z80_ram, sizeof g_machine.bus.z80_ram),
            /* reported, never compared: the scene being played ([recomp.sync] scene) */
            DK_SCENE_ADDR ? (unsigned)m68k_read32(DK_SCENE_ADDR) : 0u);
}

static void sync_init(void)
{
    const char *sl = getenv("DKIT_STACK_LOW");
    if (sl && *sl) s_stack_low = (uint32_t)strtoul(sl, NULL, 0);
    const char *path = getenv("DKIT_SYNC_CSV");
    if (!path || !path[0]) return;
    s_sync = fopen(path, "w");
    if (!s_sync) {
        fprintf(stderr, "[dkit] cannot open DKIT_SYNC_CSV %s\n", path);
        exit(2);
    }
    setvbuf(s_sync, NULL, _IOFBF, 1 << 20);
    fprintf(s_sync, "sync,frame,ram_crc32,vram_crc32,cram_crc32,vsram_crc32,z80_crc32,scene\n");
}

#ifdef DK_STEPPED
/* The main program waits for the V-int with no sync point before it ([recomp.sync] wait_vint). A
   timed run gets that V-int mid-frame; the frame-synchronous runner would spin for ever, so it
   takes the V-int there (game.toml game_hook sites; the hand C's wait is the same call). A site
   is the wait loop's backward Bcc: the V-int is taken when the branch is about to loop (its
   condition against SR), so a wait already satisfied takes none, whatever it compares (a flag,
   a counter). With `spins` > 1, only the spins-th loop-back in a row with no V-int between takes
   it: a loop whose first pass changes what it compares is not yet a wait (Batman & Robin's
   Pause_Update at $9CBA restarts the code on a mismatch and compares the same press with the
   table's first byte, in the same frame, before it spins; a V-int at the first loop-back dropped
   that press, and the section-skip code never completed on the stepped machine). With `flag`, the
   older rule: at pc, while that RAM byte is 0. Every other runner: no hook. */
static int bcc_taken(uint16_t op, uint16_t sr)
{
    int c = sr & 1, v = sr >> 1 & 1, z = sr >> 2 & 1, n = sr >> 3 & 1;
    switch ((op >> 8) & 0xF) {
    case 0x0: return 1;                 case 0x1: return 0;
    case 0x2: return !c && !z;          case 0x3: return c || z;
    case 0x4: return !c;                case 0x5: return c;
    case 0x6: return !z;                case 0x7: return z;
    case 0x8: return !v;                case 0x9: return v;
    case 0xA: return !n;                case 0xB: return n;
    case 0xC: return n == v;            case 0xD: return n != v;
    case 0xE: return !z && n == v;      default:  return z || n != v;
    }
}

/* The V-ints taken since the last sync point. The first reads the runner frame's pads (every
   button held over the sync interval: dkit.genesis.stepped); each further one, the next of the
   interval's later reference frames (the script's VPAD lines), so a pause loop that waits for a
   release and a press sees them frame by frame as the reference did; past the queue, the
   script's later frames (input_script_peek: a pause the reference never made still gets presses).
   (s_vints: above game_on_sync, which resets it.) */
#include "input_script.h"
static uint32_t s_site_spins[DK_WAIT_SITES ? DK_WAIT_SITES : 1];   /* loop-backs since the last V-int */
static void wait_vint_here(uint32_t pc)
{
    memset(s_site_spins, 0, sizeof s_site_spins);
    if (s_vints) {
        uint32_t n = input_script_vpad_count(), j = s_vints - 1;
        g_machine.bus.pad[0] = j < n ? input_script_vpad(j, 0) : input_script_peek(j - n, 0);
        g_machine.bus.pad[1] = j < n ? input_script_vpad(j, 1) : input_script_peek(j - n, 1);
    }
    s_vints++;
    x68k_wait_vint(pc);
}

static int game_instruction_hook(uint32_t pc)
{
    for (int i = 0; i < DK_WAIT_SITES; i++) {
        if (dk_wait_sites[i].pc != pc) continue;
        if (dk_wait_sites[i].flag) {
            if (m68k_read8(dk_wait_sites[i].flag) == 0) wait_vint_here(pc);
        } else {
            uint16_t op = m68k_read16(pc);
            if ((op & 0xF000) == 0x6000 && bcc_taken(op, g_cpu.SR)
                && ++s_site_spins[i] >= (dk_wait_sites[i].spins ? dk_wait_sites[i].spins : 1u))
                wait_vint_here(pc);
        }
    }
    return 0;
}

/* A stepped run that spins on a RAM flag makes no hardware read (the engine's hang guard counts
   those) and may enter no function: a watchdog thread ends it when no sync point has come for
   DKIT_STEPPED_HANG_S seconds (default 30), naming the last game function entered, so the
   driver gets a reason instead of a process burning a core (Batman & Robin: three hung for 20
   minutes on a counter wait before this). */
#ifdef _WIN32
#include <process.h>
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif
static unsigned s_hang_s = 30;
static void watchdog_body(void)
{
    uint64_t last = (uint64_t)-1;
    unsigned still = 0;
    for (;;) {
#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
        uint64_t now = x68k_sync_count();
        still = now == last ? still + 1 : 0;
        last = now;
        if (still >= s_hang_s) {
            fprintf(stderr, "[stepped] no sync point for %u s after sync %llu: stuck; last game function "
                            "entered $%06X\n", still, (unsigned long long)now, (unsigned)x68k_last_entry());
            fflush(stderr);
            /* the rows so far are evidence (two modes stopped alike compare equal up to here): the
               game thread is spinning, not writing, so the sync file can be flushed from here */
            if (s_sync) fflush(s_sync);
            _exit(3);
        }
    }
}
#ifdef _WIN32
static unsigned __stdcall watchdog_main(void *arg) { (void)arg; watchdog_body(); return 0; }
#else
static void *watchdog_main(void *arg) { (void)arg; watchdog_body(); return NULL; }
#endif
static void watchdog_start(void)
{
    const char *h = getenv("DKIT_STEPPED_HANG_S");
    if (h && *h) s_hang_s = (unsigned)strtoul(h, NULL, 0);
    if (!s_hang_s) return;
#ifdef _WIN32
    _beginthreadex(NULL, 0, watchdog_main, NULL, 0, NULL);
#else
    pthread_t t;
    pthread_create(&t, NULL, watchdog_main, NULL);
#endif
}
#endif

/* The hand C's IRQ_WAIT (dk_mem.h): on the frame-synchronous runner, a wait site's V-int, the
   pads included (wait_vint_here). x68k_wait_vint alone took every V-int of a sync interval with
   the runner frame's pads, so a C wait for a press never saw it (Batman & Robin's Pause_Update:
   the section-skip code's presses, stepped scenario-level-skip stuck at sync 1,168). Every
   other runner: x68k_wait_vint, a no-op there. */
void dk_irq_wait(void)
{
#ifdef DK_STEPPED
    wait_vint_here(0u);
#else
    x68k_wait_vint(0u);
#endif
}

/* ---- branch plans (the runner's --branch-plan; dkit genesis recomp corpus) ---------------
 * One process plays the corpus runs that share the movie's start as branches of it. The trace
 * stays one CSV in time order; marker lines between the rows say whose rows follow, with the
 * runner's cumulative counters (dispatch misses, interpreted instructions) at that moment:
 *   #save F M I      the machine snapshotted after frame F; a branch's rows follow
 *   #end TAG M I     branch TAG ended: the rows since the last #save/#restore were its own
 *   #restore F M I   the snapshot of frame F put back; another branch's rows, or the carrier's
 * Rows that no #end closes are the carrier's. The kit splits them per run (dkit genesis recomp). */

static void branch_mark(const char *what, const char *arg)
{
    /* the sync rows (phase 4) carry the same markers at the same moments */
    FILE *fs[2] = {s_trace, s_sync};
    for (int i = 0; i < 2; i++)
        if (fs[i])
            fprintf(fs[i], "#%s %s %llu %llu\n", what, arg,
                    (unsigned long long)glue_miss_count_any(), (unsigned long long)g_m68ki_insn_total);
}

static void game_branch_save(void)
{
    char f[24];
    s_saved_frame = s_last_frame;
    snprintf(f, sizeof f, "%llu", (unsigned long long)s_saved_frame);
    branch_mark("save", f);
    cov_branch_save();
#if defined(DK_HANDC)
    dk_handc_branch_save();
#endif
#ifdef DK_VERIFY
    dk_verify_branch_save();
#endif
}

static void game_branch_restore(void)
{
    char f[24];
    snprintf(f, sizeof f, "%llu", (unsigned long long)s_saved_frame);
    branch_mark("restore", f);
    cov_branch_restore();
    /* g_xa7min is machine state (the snapshot's): the restore put back the value of the save, so
       the branch just ended keeps its lowest A7 here (Batman & Robin: a fuzz branch's $FFFF4C was
       erased, the process reported $FFFF58 and D1's stack_low came out 4 bytes too high) */
    if (s_branch_a7min - 1u < g_xa7min - 1u) g_xa7min = s_branch_a7min;
#if defined(DK_HANDC)
    dk_handc_branch_restore();
#endif
#ifdef DK_VERIFY
    dk_verify_branch_restore();
#endif
}

static void game_branch_end(const char *tag)
{
    branch_mark("end", tag);
    cov_write(tag);
    if (g_xa7min - 1u < s_branch_a7min - 1u) s_branch_a7min = g_xa7min;
#ifdef DK_VERIFY
    dk_verify_branch_end(tag);
#endif
}

const GameSpec g_game_spec = {
    .display_name           = "The Adventures of Batman and Robin",
    .short_name             = "BATMANROBIN",

    .expected_rom_crc32     = 0x0CAAA4ACu,
    .expected_rom_size      = 0x200000u,

    /* Discovery is the disassembly: a dispatch miss is a discovery bug and must stay loud. */
    .tier3_floor_default    = 0,

    /* Frame-identical to the reference emulator: the exact scheduler, with code generated by
     * game.toml exact_timing = true. */
    .exact_timing           = 1,
    /* The reference emulator's release where exact timings differ (dkit.toml [emulator]
     * bizhawk: 24 = 2.4, 291 = 2.9.1). */
    .exact_reference        = 291,

    /* Logical positions (phase 4): the game functions are the disassembly's (XENTRY counts their
     * entries: game.toml entry_hooks), a sync point is a main-program entry to DK_SYNC_PC. The
     * stepped runners are this spec built with DK_STEPPED over code generated --untimed. */
    .game_functions         = dk_game_functions,
    .game_function_count    = (unsigned)(sizeof dk_game_functions / sizeof dk_game_functions[0]),
    .sync_pc                = DK_SYNC_PC,
    .on_sync                = game_on_sync,
#ifdef DK_STEPPED
    .stepped                = 1,
    .fsync_vint_at_wait     = DK_FSYNC_VINT_AT_WAIT,
    /* The H-int raster on the frame-synchronous machine (phase-5 ruling R3): each display after
     * a V-int is drawn line by line, the game's H-int handler (the RAM raster at $FFE560, its jmp
     * stubs to 01C468 / 024526 / 0245E0) taken at each line the VDP's counter says, so the
     * stage-2 boss spans, the Joker balloon restore and Stage2_ArenaHIntSplit show (engine
     * fsync-raster; docs/NOTES.md "Phase 5: the raster on the frame-synchronous machine"). */
    .fsync_raster           = DK_FSYNC_RASTER,
    .instruction_hook       = game_instruction_hook,
#endif
#ifdef DK_VERIFY
    .on_entry               = dk_verify_entry,         /* the shadow tests (D4) */
#endif

    .call_entry_point       = game_call_entry_point,
    .call_vblank            = game_call_vblank,
    .call_hblank            = game_call_hblank,
    .resume_main_loop_pc    = 0u,
    .save_resume_pc         = NULL,
    .dispatch_main_loop_pc  = 0u,
    .call_periodic          = NULL,

    .on_post_reset          = NULL,
    .on_frame_pre           = NULL,
    .on_frame_post          = game_on_frame_post,
    .on_hblank              = NULL,

    .handle_arg             = NULL,
    .arg_usage              = NULL,
    .dispatch_override      = NULL,

    .fill_frame_record      = NULL,
    .frame_record_version   = 0u,

    .commands               = NULL,
    .command_count          = 0,

    .branch_save            = game_branch_save,
    .branch_restore         = game_branch_restore,
    .branch_end             = game_branch_end,
};
