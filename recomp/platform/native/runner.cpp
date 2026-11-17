/* runner.cpp - the native runner's main (phase 5 P6): The Adventures of Batman and Robin on the
 * native machine, no 68000. Played: a window, the keyboard and controllers, the sound. Driven by the
 * kit (dkit genesis recomp stepped / stepped-corpus --mode native, package), with the command line
 * and outputs of the engine's runner, so every check reads it as it read the engine-based one:
 *
 *   batmanrobin_native ROM [--benchmark N] [--input-script F] [--no-render] [--hash-frames K]
 *                          [--branch-plan F] [--fresh-evidence]
 *   --benchmark N      play N runner frames headless, then GENESISRECOMP_BENCHMARK {json} on stdout
 *   --input-script F   the pads per runner frame (input_script.cpp)
 *   --no-render        no picture is drawn (a trace that compares none)
 *   --hash-frames K    [FBHASH] frame=f w=W h=H hash=0x... every K runner frames (FNV-1a-64 of the
 *                      frame's active region, as the engine's main.c framebuf_active_hash)
 *   --branch-plan F    (--benchmark) other runs that start with this one's pads, played from a
 *                      snapshot of this run (`FRAME FRAMES TAG SCRIPT` lines: main.c "Branch plans")
 * Phase 6 (docs/phases/P6-enhance.md, the contract in docs/NOTES.md "Phase 6: the contract"):
 *   --options K=V,...  the enhancement options (options.cpp; also settings.ini [options] beside the
 *                      exe, the command line winning); --options --json lists them and exits 0
 *   --present-log F    (--benchmark) the present clock's rows to F: sync,t_us,whole,gameplay,fb_fnv1a64
 *   --flip N           (--benchmark) player 1's C button inverted in runner frames N..N+7 (latency probe)
 *   --game-rows        (--benchmark) game_crc32 and gameplay beside each sync row (enhance.cpp)
 *   --hash-centre      (--benchmark) fb_centre beside each sync row: the centre 320 columns' FNV-1a-64
 *   --render-from F    (--benchmark) no picture before runner frame F (a probe's fast lead-in)
 *   --audio-hash       (--benchmark) the sound output made headless and hashed (audio counters)
 *   --suppress-sfx     (--benchmark) the effect window's writes dropped (E6's reference run)
 *   At exit: [dkit] options {...} (the effective values) and [dkit] enhance {...} (enhance.h).
 * Environment: DKIT_SYNC_CSV (the sync rows), DKIT_STACK_LOW, DKIT_SYNC_DUMP + DKIT_DUMP_DIR,
 * DKIT_STEPPED_HANG_S, DKIT_EXPORT_SELFTEST (export.cpp), as spec.c reads them.
 *
 * The sync rows (phase 4's columns) come only from the exporter's image (game/runtime/export.cpp):
 *   sync,frame,ram_crc32,vram_crc32,cram_crc32,vsram_crc32,z80_crc32,scene
 * RAM with the stack area [stack_low, top) and [recomp] dont_care zeroed. At exit:
 *   [dkit] counters {"dispatch_misses": 0, "interp_instructions": 0, ..., "exported": {...}}
 * (the flow runtime makes an unknown code address fatal, and nothing interprets: both are 0).
 *
 * The ROM is the user's (ruling R2): its SHA-1 is checked before anything runs; any other file is
 * refused (exit 2).
 *
 * Adapted in part from segagenesisrecomp (runner/main.c's frame loop, branch plans and benchmark
 * line) and this game's spec.c, PolyForm Noncommercial License 1.0.0, Copyright (c) 2026 Matthew
 * Stan (https://polyformproject.org/licenses/noncommercial/1.0.0).
 */
#include <chrono>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <thread>
#include <vector>

extern "C" {
#include "video/genesis_machine.h"
#include "audio/event_queue.h"
}
#include "dkit_sync.h"
#include "runtime/dk_image.h"
#include "native_machine.h"
#include "host_sdl.h"
#include "options.h"
#include "enhance.h"
#include "present.h"

#ifdef _WIN32
#include <io.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>   /* the watchdog's clock: GetThreadTimes */
#define DK_EXIT_FAST _exit
#else
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#define DK_EXIT_FAST _exit
#endif

uint8_t g_rom[0x400000];

#define DK_ROM_SHA1 "1da4b3fc92a6572e70d748ebad346aeb291f89f3"
#define DK_ROM_SIZE 0x200000u
#define DK_TITLE "The Adventures of Batman and Robin"

/* ======================================================================== */
/* SHA-1 (the ROM check) */

static void sha1_block(uint32_t h[5], const uint8_t *p)
{
    uint32_t w[80];
    for (int i = 0; i < 16; i++)
        w[i] = (uint32_t)p[4 * i] << 24 | (uint32_t)p[4 * i + 1] << 16 | (uint32_t)p[4 * i + 2] << 8 | p[4 * i + 3];
    for (int i = 16; i < 80; i++) {
        uint32_t x = w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16];
        w[i] = x << 1 | x >> 31;
    }
    uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4];
    for (int i = 0; i < 80; i++) {
        uint32_t f, k;
        if (i < 20)      { f = (b & c) | (~b & d);           k = 0x5A827999u; }
        else if (i < 40) { f = b ^ c ^ d;                    k = 0x6ED9EBA1u; }
        else if (i < 60) { f = (b & c) | (b & d) | (c & d);  k = 0x8F1BBCDCu; }
        else             { f = b ^ c ^ d;                    k = 0xCA62C1D6u; }
        uint32_t t = (a << 5 | a >> 27) + f + e + k + w[i];
        e = d; d = c; c = b << 30 | b >> 2; b = a; a = t;
    }
    h[0] += a; h[1] += b; h[2] += c; h[3] += d; h[4] += e;
}

static void sha1_hex(const uint8_t *p, size_t n, char out[41])
{
    uint32_t h[5] = {0x67452301u, 0xEFCDAB89u, 0x98BADCFEu, 0x10325476u, 0xC3D2E1F0u};
    size_t i = 0;
    for (; i + 64 <= n; i += 64) sha1_block(h, p + i);
    uint8_t last[128] = {0};
    size_t r = n - i;
    memcpy(last, p + i, r);
    last[r] = 0x80;
    size_t len = r + 1 + 8 <= 64 ? 64 : 128;
    uint64_t bits = (uint64_t)n * 8u;
    for (int k = 0; k < 8; k++) last[len - 1 - k] = (uint8_t)(bits >> (8 * k));
    for (size_t o = 0; o < len; o += 64) sha1_block(h, last + o);
    for (int k = 0; k < 5; k++) snprintf(out + 8 * k, 9, "%08x", (unsigned)h[k]);
}

static void load_rom(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "[native] cannot open the ROM %s\n", path);
        exit(2);
    }
    size_t n = fread(g_rom, 1, sizeof g_rom, f);
    int more = fgetc(f) != EOF;
    fclose(f);
    char got[41] = "";
    sha1_hex(g_rom, n, got);
    if (n != DK_ROM_SIZE || more || strcmp(got, DK_ROM_SHA1)) {
        fprintf(stderr, "[native] this ROM's SHA-1 is %s (%zu bytes), not %s (%s): refusing to run. Use your own "
                        "dump of the cartridge.\n", got, n, DK_ROM_SHA1, DK_TITLE);
        fflush(stderr);
        exit(2);
    }
}

/* ======================================================================== */
/* Sync rows (spec.c, DK_NATIVE: from the exporter's image only) */

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

static uint32_t crc32_bytes(const uint8_t *p, size_t n) { return crc32_update(0xFFFFFFFFu, p, n) ^ 0xFFFFFFFFu; }

static FILE *s_sync;
static uint64_t s_sync_rows;
static uint32_t s_stack_low = DK_STACK_LOW;

typedef struct RamSeg { uint32_t lo, hi; int masked; } RamSeg;
static uint8_t s_crc_mask[0x10000];
static RamSeg s_segs[2 * DK_DONT_CARE + 4];
static int s_nsegs = -1;

static void ram_mask_build(void)
{
    uint32_t lo = s_stack_low & 0xFFFFu, hi = (DK_STACK_TOP - 1u) & 0xFFFFu;
    memset(s_crc_mask, 0, sizeof s_crc_mask);
    if ((s_stack_low & 0xFF0000u) == 0xFF0000u && hi >= lo)   /* stack_low not measured: no stack mask */
        memset(s_crc_mask + lo, 1, hi + 1u - lo);
    for (int i = 0; i < DK_DONT_CARE; i++)
        memset(s_crc_mask + dk_dont_care[i].lo, 1, dk_dont_care[i].hi - dk_dont_care[i].lo);
    s_nsegs = 0;
    for (uint32_t a = 0; a < 0x10000u;) {
        uint32_t b = a;
        while (b < 0x10000u && s_crc_mask[b] == s_crc_mask[a]) b++;
        s_segs[s_nsegs].lo = a;
        s_segs[s_nsegs].hi = b;
        s_segs[s_nsegs++].masked = s_crc_mask[a];
        a = b;
    }
}

static uint32_t ram_crc_masked(const uint8_t *ram)
{
    static const uint8_t zeros[256] = {0};
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
    for (const char *p = list; *p;) {
        char *e;
        unsigned long long n = strtoull(p, &e, 10);
        if (e == p) break;
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
            if (f) { fwrite(vram, 1, DK_IMAGE_VRAM_BYTES, f); fclose(f); }
        }
        p = *e ? e + 1 : e;
    }
}

/* Phase 6's columns beside a sync row (--game-rows, --hash-centre): the row waits for the end of its
   runner frame (fb_centre is that frame's picture, as --fb's hash is), then gets them. Without
   them, the row is written at once, as phase 5's. */
static int s_extra_cols;
static char s_row[200];
static int s_row_pending;
static uint32_t s_row_game;
static int s_last_gameplay;

static void row_flush(int have_frame)
{
    if (!s_row_pending || !s_sync) return;
    s_row_pending = 0;
    fputs(s_row, s_sync);
    if (g_enh_flags.game_rows) fprintf(s_sync, ",%08x,%d", s_row_game, s_last_gameplay);
    if (g_enh_flags.hash_centre) {
        if (have_frame) { fprintf(s_sync, ",%016llX", (unsigned long long)enh_centre_hash()); enh_centre_shot(s_sync_rows); }
        else fputs(",", s_sync);
    }
    fputc('\n', s_sync);
}

extern "C" void nm_on_sync(uint64_t sync, uint64_t frame)
{
    s_sync_rows = sync;
    static DkImage im;
    dk_export(&im);
    maybe_dump_sync(sync, im.ram, im.vram);
    s_last_gameplay = enh_gameplay(im.ram);
    if (s_last_gameplay) g_enh.gameplay_frames++;
    if (!s_sync) return;
    uint32_t sa = DK_SCENE_ADDR & 0xFFFFu;
    snprintf(s_row, sizeof s_row, "%llu,%llu,%08x,%08x,%08x,%08x,%08x,%08x", (unsigned long long)sync,
             (unsigned long long)frame, ram_crc_masked(im.ram), crc32_bytes(im.vram, sizeof im.vram),
             crc32_bytes(im.cram, sizeof im.cram), crc32_bytes(im.vsram, sizeof im.vsram),
             crc32_bytes(im.z80, sizeof im.z80),
             DK_SCENE_ADDR ? (unsigned)((uint32_t)im.ram[sa] << 24 | (uint32_t)im.ram[(sa + 1u) & 0xFFFFu] << 16
                                        | (uint32_t)im.ram[(sa + 2u) & 0xFFFFu] << 8 | im.ram[(sa + 3u) & 0xFFFFu]) : 0u);
    if (!s_extra_cols) {
        fputs(s_row, s_sync);
        fputc('\n', s_sync);
        return;
    }
    if (s_row_pending) row_flush(0);   /* two sync points in one runner frame: not the kit's machine */
    if (g_enh_flags.game_rows) {
        if (s_nsegs < 0) ram_mask_build();
        s_row_game = enh_game_crc(&im, s_crc_mask);
    }
    s_row_pending = 1;
}

static void sync_init(void)
{
    const char *sl = getenv("DKIT_STACK_LOW");
    if (sl && *sl) s_stack_low = (uint32_t)strtoul(sl, nullptr, 0);
    const char *path = getenv("DKIT_SYNC_CSV");
    if (!path || !path[0]) return;
    s_sync = fopen(path, "w");
    if (!s_sync) {
        fprintf(stderr, "[dkit] cannot open DKIT_SYNC_CSV %s\n", path);
        exit(2);
    }
    setvbuf(s_sync, nullptr, _IOFBF, 1 << 20);
    s_extra_cols = g_enh_flags.game_rows || g_enh_flags.hash_centre;
    fprintf(s_sync, "sync,frame,ram_crc32,vram_crc32,cram_crc32,vsram_crc32,z80_crc32,scene%s%s\n",
            g_enh_flags.game_rows ? ",game_crc32,gameplay" : "", g_enh_flags.hash_centre ? ",fb_centre" : "");
}

static void report_counters(void)
{
    row_flush(0);
    if (s_sync) fflush(s_sync);
    enh_present_close();
    char exported[640];
    dk_export_stats(exported, (int)sizeof exported);
    fprintf(stderr, "[dkit] counters {\"dispatch_misses\": 0, \"interp_instructions\": 0, \"stub_hits\": 0, "
                    "\"sync_points\": %llu, \"a7_min\": null, \"stack_low\": %u, \"exported\": %s}\n",
            (unsigned long long)s_sync_rows, (unsigned)(s_stack_low & 0xFFFFFFu), exported);
    fprintf(stderr, "[dkit] options %s\n", options_effective_json().c_str());
    fprintf(stderr, "[dkit] enhance %s\n", enh_json(1).c_str());
    fflush(stderr);
}

extern "C" void dk_install_crash_trace(void);   /* game/runtime/crash_win.cpp */
extern "C" void nm_game_start(void) {}

/* A stopped run: the reason as the kit reads it ("[stepped] ...", last_words), the exporter's counts
   (no counters line: _exit skips atexit), the rows so far, exit 3. */
static void stop_stuck(const char *why)
{
    char exported[640];
    dk_export_stats(exported, (int)sizeof exported);
    fprintf(stderr, "[dkit] exported %s\n", exported);
    fprintf(stderr, "[stepped] %s: stuck; last game function entered $%06X\n", why, (unsigned)nm_last_entry());
    fflush(stderr);
    row_flush(0);   /* the pending row (phase 6's columns) */
    if (s_sync) fflush(s_sync);
    DK_EXIT_FAST(3);
}

/* machine.cpp: a sync interval longer than its input (nm_wait_vint's bound), on the game's thread */
extern "C" void nm_stuck(const char *why) { stop_stuck(why); }

/* The game thread's CPU seconds (the watchdog's clock): a process the machine starves of CPU uses
   none, so its wait is never counted as the game's. */
#ifdef _WIN32
static HANDLE s_game_thread;
static void game_thread_capture(void)
{
    DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &s_game_thread,
                    THREAD_QUERY_LIMITED_INFORMATION, FALSE, 0);
}
static double game_thread_cpu_s(void)
{
    FILETIME c, e, k, u;
    if (!s_game_thread || !GetThreadTimes(s_game_thread, &c, &e, &k, &u)) return -1.0;
    auto s = [](const FILETIME &t) { return (double)(((uint64_t)t.dwHighDateTime << 32) | t.dwLowDateTime) / 1e7; };
    return s(k) + s(u);
}
#else
static clockid_t s_game_clock;
static int s_game_clock_ok;
static void game_thread_capture(void) { s_game_clock_ok = pthread_getcpuclockid(pthread_self(), &s_game_clock) == 0; }
static double game_thread_cpu_s(void)
{
    struct timespec ts;
    if (!s_game_clock_ok || clock_gettime(s_game_clock, &ts) != 0) return -1.0;
    return (double)ts.tv_sec + ts.tv_nsec / 1e9;
}
#endif

/* The watchdog (DKIT_STEPPED_HANG_S, default 30). A run that spins on a RAM flag makes no hardware
   read and enters no function; one that waits V-int by V-int at an IRQ_WAIT site (a pause) is
   playing frames. So the game's progress is nm_progress(), its sync points and IRQ_WAIT V-ints
   (game time), and the time it is measured against is the game thread's own CPU time: the run is
   stuck when the game thread has spent hang_s seconds of CPU with neither. A sync interval that
   plays past its input's last reference frame (a pause nothing ends) is machine.cpp's bound.
   Until 2026-10-30 this counted wall seconds without a sync point: a corpus pass's runners at
   below-normal priority went 45-98 s without CPU while the machine was saturated, and every
   fuzz-*-s1 branch has a 1,779-V-int sync interval (the fuzz's $7FF press pauses the game; it
   takes ~1-3 s alone), so those branches stopped "stuck" at random (NOTES, "The native watchdog").
   A game thread that gets CPU but is blocked outside the game (waiting, not spinning) is reported
   after 20 x hang_s wall seconds with no progress. */
static void watchdog_start(void)
{
    unsigned hang_s = 30;
    const char *h = getenv("DKIT_STEPPED_HANG_S");
    if (h && *h) hang_s = (unsigned)strtoul(h, nullptr, 0);
    if (!hang_s) return;
    game_thread_capture();
    std::thread([hang_s] {
        uint64_t last = nm_progress();
        double cpu0 = game_thread_cpu_s();
        unsigned wall = 0;
        for (;;) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            uint64_t now = nm_progress();
            double cpu = game_thread_cpu_s();
            if (now != last) {
                last = now;
                cpu0 = cpu;
                wall = 0;
                continue;
            }
            wall++;
            double spent = cpu >= 0 && cpu0 >= 0 ? cpu - cpu0 : (double)wall;   /* no thread clock: wall */
            char why[256];
            if (spent >= hang_s) {
                snprintf(why, sizeof why, "no sync point or V-int for %.0f s of the game thread's CPU (%u s) after "
                                          "sync %llu", spent, wall, (unsigned long long)nm_sync_count());
                stop_stuck(why);
            }
            if (wall >= 20u * hang_s) {
                snprintf(why, sizeof why, "no sync point or V-int for %u s after sync %llu, the game thread "
                                          "blocked (%.1f s of CPU in them)", wall, (unsigned long long)nm_sync_count(),
                         spent);
                stop_stuck(why);
            }
        }
    }).detach();
}

/* ======================================================================== */
/* The controller ports (settings.ini [input.pN] pad_type, as the kit writes it for the movie) */

static void read_pad_types(const char *exe_dir, int types[2])
{
    types[0] = types[1] = 0;
    std::string paths[2] = { "settings.ini", std::string(exe_dir) + "/settings.ini" };
    for (const std::string &p : paths) {
        FILE *f = fopen(p.c_str(), "r");
        if (!f) continue;
        char line[256];
        int port = -1;
        while (fgets(line, sizeof line, f)) {
            int n;
            if (sscanf(line, " [input.p%d]", &n) == 1) { port = n - 1; continue; }
            if (line[0] == '[') { port = -1; continue; }
            int v;
            if (port >= 0 && port < 2 && sscanf(line, " pad_type = %d", &v) == 1) types[port] = v;
        }
        fclose(f);
        return;
    }
}

/* ======================================================================== */
/* The runner frame */

static uint64_t s_ticks;      /* runner frames played (the snapshot's own: restored with it) */
static int s_types[2];

static uint16_t frame_pad(int port)
{
    uint16_t m = ns_player_mask(port);
    uint16_t p = (uint16_t)(m & 0xFFu);
    if (s_types[port] == 1) p |= (uint16_t)(m & 0xF00u);
    return p;
}

static uint64_t fb_hash(int *w, int *h)
{
    int stride;
    const uint32_t *fb = nm_framebuffer(&stride, w, h);
    uint64_t hash = 0xCBF29CE484222325ULL;
    const uint8_t *p = (const uint8_t *)fb;
    int row_bytes = *w * (int)sizeof(uint32_t);
    for (int y = 0; y < *h; y++) {
        const uint8_t *row = p + (size_t)y * stride * sizeof(uint32_t);
        for (int x = 0; x < row_bytes; x++) { hash ^= row[x]; hash *= 0x100000001B3ULL; }
    }
    return hash;
}

/* ---- branch plans (main.c "Branch plans") ---- */

struct BranchItem { uint32_t at, frames; std::string tag, script; };
static std::vector<BranchItem> s_br;
static size_t s_br_i;
static int s_br_active;
static uint32_t s_br_end, s_br_saved_at = UINT32_MAX;
static uint8_t *s_br_blob;
static size_t s_br_len;
static const char *s_br_carrier;
static std::chrono::steady_clock::time_point s_br_t0;
static uint64_t s_last_frame, s_saved_frame;

static int branch_plan_load(const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f) { fprintf(stderr, "[branch] cannot read %s\n", path); return 0; }
    char line[800];
    while (fgets(line, sizeof line, f)) {
        if (line[0] == '#' || strspn(line, " \t\r\n") == strlen(line)) continue;
        unsigned at, frames;
        char tag[128];
        int used = 0;
        if (sscanf(line, "%u %u %127s %n", &at, &frames, tag, &used) != 3 || !used) {
            fprintf(stderr, "[branch] %s: bad line: %s", path, line);
            fclose(f);
            return 0;
        }
        std::string script = line + used;
        script.erase(script.find_last_not_of("\r\n") + 1);
        if (script.empty() || (!s_br.empty() && at < s_br.back().at)) {
            fprintf(stderr, "[branch] %s: no script, or a line before the previous one's frame: %s", path, line);
            fclose(f);
            return 0;
        }
        s_br.push_back({ at, frames, tag, script });
    }
    fclose(f);
    fprintf(stderr, "[branch] %zu branches from %s\n", s_br.size(), path);
    return 1;
}

static void branch_mark(const char *what, const char *arg)
{
    if (s_sync) fprintf(s_sync, "#%s %s 0 0\n", what, arg);
}

static void branch_script_seek(const char *script, uint32_t frame)
{
    if (!ns_load(script)) {
        fprintf(stderr, "[branch] %s: not an input-only script\n", script);
        exit(2);
    }
    for (uint32_t f = 0; f <= frame; f++) ns_tick(f);
}

static void branch_tick(uint32_t *frame_num)
{
    for (;;) {
        if (s_br_active) {
            if (*frame_num < s_br_end) return;
            const BranchItem &b = s_br[s_br_i];
            branch_mark("end", b.tag.c_str());
            printf("GENESISRECOMP_BRANCH {\"tag\": \"%s\", \"at\": %u, \"frames\": %u, \"complete\": true, \"seconds\": %.3f, "
                   "\"enhance\": %s}\n",
                   b.tag.c_str(), (unsigned)b.at, (unsigned)b.frames,
                   std::chrono::duration<double>(std::chrono::steady_clock::now() - s_br_t0).count(),
                   enh_branch_end().c_str());
            fflush(stdout);
            if (!nm_snapshot_load(s_br_blob, s_br_len)) {
                fprintf(stderr, "[branch] restoring the snapshot of frame %u failed\n", (unsigned)s_br_saved_at);
                exit(2);
            }
            s_ticks = s_br_saved_at;
            char f[24];
            snprintf(f, sizeof f, "%llu", (unsigned long long)s_saved_frame);
            branch_mark("restore", f);
            *frame_num = s_br_saved_at;
            s_br_active = 0;
            s_br_i++;
            if (s_br_i >= s_br.size() || s_br[s_br_i].at != *frame_num) {
                branch_script_seek(s_br_carrier, *frame_num);
                return;
            }
        }
        if (s_br_i >= s_br.size() || s_br[s_br_i].at != *frame_num) return;
        const BranchItem &b = s_br[s_br_i];
        if (s_br_saved_at != *frame_num) {
            free(s_br_blob);
            s_br_len = nm_snapshot_save(&s_br_blob);
            s_br_saved_at = *frame_num;
            s_saved_frame = s_last_frame;
            char f[24];
            snprintf(f, sizeof f, "%llu", (unsigned long long)s_saved_frame);
            branch_mark("save", f);
        }
        fprintf(stderr, "[branch] %s from frame %u, %u frames\n", b.tag.c_str(), (unsigned)b.at, (unsigned)b.frames);
        branch_script_seek(b.script.c_str(), *frame_num);
        s_br_end = *frame_num + b.frames;
        s_br_active = 1;
        enh_branch_begin();
        s_br_t0 = std::chrono::steady_clock::now();
    }
}

/* ======================================================================== */

static void usage(void)
{
    fprintf(stderr, "usage: batmanrobin_native ROM [--benchmark N] [--input-script F] [--no-render] [--hash-frames K] "
                    "[--branch-plan F] [--options K=V,...] [--present-log F] [--flip N] [--game-rows] [--hash-centre] "
                    "[--render-from F] [--audio-hash] [--suppress-sfx]\n"
                    "       batmanrobin_native --options --json\n");
}

static int play(void);

int main(int argc, char **argv)
{
    options_init(&argc, argv);   /* settings.ini [options], then --options (exits after --options --json) */
    const char *rom = nullptr, *script = nullptr, *plan = nullptr, *present_log = nullptr;
    uint32_t benchmark = 0, hash_frames = 0;
    int no_render = 0;
    for (int i = 1; i < argc; i++) {
        const char *a = argv[i];
        if (!strcmp(a, "--benchmark") && i + 1 < argc) benchmark = (uint32_t)strtoul(argv[++i], nullptr, 0);
        else if (!strcmp(a, "--input-script") && i + 1 < argc) script = argv[++i];
        else if (!strcmp(a, "--hash-frames") && i + 1 < argc) hash_frames = (uint32_t)strtoul(argv[++i], nullptr, 0);
        else if (!strcmp(a, "--branch-plan") && i + 1 < argc) plan = argv[++i];
        else if (!strcmp(a, "--no-render")) no_render = 1;
        else if (!strcmp(a, "--present-log") && i + 1 < argc) present_log = argv[++i];
        else if (!strcmp(a, "--flip") && i + 1 < argc) g_enh_flags.flip = strtol(argv[++i], nullptr, 0);
        else if (!strcmp(a, "--render-from") && i + 1 < argc) g_enh_flags.render_from = strtol(argv[++i], nullptr, 0);
        else if (!strcmp(a, "--game-rows")) g_enh_flags.game_rows = 1;
        else if (!strcmp(a, "--hash-centre")) g_enh_flags.hash_centre = 1;
        else if (!strcmp(a, "--audio-hash")) g_enh_flags.audio_hash = 1;
        else if (!strcmp(a, "--suppress-sfx")) g_enh_flags.suppress_sfx = 1;
        else if (!strcmp(a, "--fresh-evidence")) {}
        else if (a[0] == '-' && a[1] == '-') fprintf(stderr, "[native] ignoring %s\n", a);
        else if (!rom) rom = a;
        else { usage(); return 2; }
    }
    if (!rom) { usage(); return 2; }
    load_rom(rom);   /* refuses anything but the cartridge (exit 2) */

    std::string exe_dir = argv[0];
    size_t cut = exe_dir.find_last_of("/\\");
    exe_dir = cut == std::string::npos ? "." : exe_dir.substr(0, cut);
    read_pad_types(exe_dir.c_str(), s_types);

    g_enh_flags.present_log = present_log != nullptr;
    if (!benchmark && (present_log || g_enh_flags.flip >= 0 || g_enh_flags.game_rows || g_enh_flags.hash_centre ||
                       g_enh_flags.render_from || g_enh_flags.audio_hash || g_enh_flags.suppress_sfx)) {
        fprintf(stderr, "--present-log, --flip, --game-rows, --hash-centre, --render-from, --audio-hash and "
                        "--suppress-sfx are for --benchmark runs\n");
        return 2;
    }
    if (plan && (present_log || g_enh_flags.flip >= 0 || g_enh_flags.render_from)) {
        fprintf(stderr, "--present-log, --flip and --render-from are for runs without --branch-plan\n");
        return 2;
    }
    if (no_render && (hash_frames || g_enh_flags.hash_centre || present_log)) {
        fprintf(stderr, "--no-render with --hash-frames, --hash-centre or --present-log: there is no picture to hash\n");
        return 2;
    }

    dk_install_crash_trace();
    crc_init();
    sync_init();
    nm_power_on();
    nm_set_pad_types(s_types[0], s_types[1]);
    if (!benchmark) return play();

    nm_set_render(!no_render);
    enh_present_init(present_log);
    hf_init();   /* high_fps: present.cpp's interpolator for the clock's frames in between */
    atexit(report_counters);
    watchdog_start();
    if (script && !ns_load(script)) return 2;
    if (plan) {
        if (!script) { fprintf(stderr, "--branch-plan needs --benchmark N and --input-script\n"); return 2; }
        if (!branch_plan_load(plan)) return 2;
        s_br_carrier = script;
    }
    ns_tick(0);   /* directive time 0 is for frame 1 (main.c: the exact runners tick once first) */
    uint32_t frame_num = 0;
    auto t0 = std::chrono::steady_clock::now();
    const int timed = g_enh.present.active;   /* the render timer: with the present clock only */
    for (;;) {
        if (frame_num >= benchmark && !s_br_active) break;
        uint32_t f = frame_num + 1;   /* the runner frame about to run ([FBHASH] frame=f) */
        uint16_t p1 = frame_pad(0);
        if (g_enh_flags.flip >= 0 && f >= (uint32_t)g_enh_flags.flip && f <= (uint32_t)g_enh_flags.flip + 7u)
            p1 ^= GPAD_C;
        int rendered = !no_render && !(g_enh_flags.render_from && f < (uint32_t)g_enh_flags.render_from);
        if (g_enh_flags.render_from && !no_render) nm_set_render(rendered);
        nm_set_pads(p1, frame_pad(1));
        auto tf = timed ? std::chrono::steady_clock::now() : t0;
        nm_run_frame();
        if (g_enh_audio_frame) g_enh_audio_frame(f);   /* the sound side's (--audio-hash, the counters) */
        audio_event_queue_reset();   /* headless: nothing is heard */
        if (s_extra_cols) row_flush(rendered);
        if (timed)
            enh_present_frame(f, s_last_gameplay, rendered,
                              std::chrono::duration<double, std::micro>(std::chrono::steady_clock::now() - tf).count());
        hf_frame_done();   /* this frame's last display is R_(k-1) for the next */
        s_last_frame = ++s_ticks;
        frame_num++;
        ns_tick(frame_num);
        if (ns_should_exit()) {
            fprintf(stderr, "[input_script] exiting per script directive (code=%d)\n", ns_exit_code());
            return ns_exit_code();
        }
        if (!s_br.empty()) branch_tick(&frame_num);
        if (hash_frames && frame_num % hash_frames == 0) {
            int w, h;
            uint64_t hh = fb_hash(&w, &h);
            fprintf(stderr, "[FBHASH] frame=%u w=%d h=%d hash=0x%016llX\n", frame_num, w, h, (unsigned long long)hh);
        }
    }
    double secs = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
    fprintf(stderr, "[DONE] %u frames completed\n", frame_num);
    printf("GENESISRECOMP_BENCHMARK {\"game\":\"BATMANROBIN\",\"frames\":%u,\"seconds\":%.9f,\"fps\":%.3f,"
           "\"ms_per_frame\":%.6f,\"requested_frames\":%u,\"complete\":%s,\"runner\":\"native\"}\n",
           frame_num, secs, secs > 0 ? frame_num / secs : 0.0, frame_num ? secs * 1000.0 / frame_num : 0.0, benchmark,
           frame_num == benchmark ? "true" : "false");
    fflush(stdout);
    return 0;
}

/* ======================================================================== */
/* Played: the window, the controllers, the sound; one host frame per displayed frame */

static int play(void)
{
    if (!host_open(DK_TITLE)) return 1;
    nm_set_play(1);
    nm_set_render(1);
    hf_init();
    uint64_t frame = 0, clock_from = 0;
    int clock_on = 0;
    static std::vector<uint32_t> between((size_t)512 * 480);
    const double T_US = 896040.0 * 1e6 / 53693175.0;   /* one NTSC frame: the present clock's unit */
    for (;;) {
        uint16_t pads[2];
        host_input(pads);   /* the pads are read when the 60 Hz build reads them: right after the last frame */
        nm_set_pads(s_types[0] == 1 ? pads[0] : (uint16_t)(pads[0] & 0xFFu),
                    s_types[1] == 1 ? pads[1] : (uint16_t)(pads[1] & 0xFFu));
        nm_run_frame();
        if (nm_game_stopped()) { fprintf(stderr, "[native] the game stopped\n"); return 1; }
        host_audio(++frame);
        int stride, w, h;
        const uint32_t *fb = nm_framebuffer(&stride, &w, &h);
        if (g_opt.high_fps > 0) {
            /* high_fps (R3): R_k is made; the clock's ticks between (k-1)T and kT show it moved back
               toward R_(k-1) (present.cpp), then R_k at kT (host_present_at waits for each time) */
            if (!clock_on) { clock_on = 1; clock_from = frame - 1; host_present_clock_reset(); }
            uint64_t k = frame - clock_from;
            if (k >= 2) {
                double t0 = (double)(k - 1) * T_US, t1 = (double)k * T_US, step = 1e6 / g_opt.high_fps;
                for (double t = (double)(long long)(t0 / step + 1.0) * step; t < t1; t += step)
                    if (t > t0 && hf_interpolate(k, (t - t0) / T_US, between.data(), 512, w, h))
                        host_present_at(between.data(), 512, w, h, t);
            }
            host_present_at(fb, stride, w, h, (double)k * T_US);
        } else {
            if (clock_on) { clock_on = 0; host_present_clock_reset(); }
            host_present(fb, stride, w, h);
        }
        hf_frame_done();
    }
}
