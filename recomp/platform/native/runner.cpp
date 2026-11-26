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
 *   --record-inputs F  (played) the session's pads written to F at its end, as an input script that
 *                      replays it (crash_report.h); with BR_CAPTURE / BR_WAV a replay renders it to
 *                      a video and the sound's stems at any options (capture.cpp)
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
 * refused (exit 2). The dump's form is found by its bytes, never its name: plain (.md/.bin/.gen), a
 * 512-byte copier header, SMD or MGD interleave (rom_check).
 *
 * The front door (the X-Men 2 harvest, docs/NOTES.md "The front door"): a played run (no --benchmark)
 * closes the console window a double-click opened (alone on its console: FreeConsole), and says what a
 * player must know in message boxes (--no-messagebox, BR_NO_MESSAGEBOX=1, GENESIS_NO_LAUNCHER=1 or a
 * dummy video driver print them instead). Started with no ROM: the launcher (launcher.cpp; not with
 * --no-launcher, [launcher] show = 0, an input script or GENESIS_NO_LAUNCHER=1; --launcher always),
 * which hands back the ROM; else settings.ini's [rom] path, a dump beside the exe, or a file picker.
 *   BR_PAD_SELFTEST=1     the controllers' slots and bindings with virtual controllers (host_sdl.cpp)
 *   BR_LAUNCHER_TEST=F    the launcher driven by a script, headless; BR_LAUNCHER_DRYRUN=1 prints its plan
 *   BR_PICK_ROM_TEST=P    the file picker's answer ("" = cancelled)
 *
 * Adapted in part from segagenesisrecomp (runner/main.c's frame loop, branch plans and benchmark
 * line) and this game's spec.c, PolyForm Noncommercial License 1.0.0, Copyright (c) 2026 Matthew
 * Stan (https://polyformproject.org/licenses/noncommercial/1.0.0).
 */
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <system_error>
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
#include "crash_report.h"
#include "capture.h"
#include "crc32_fast.h"
#include "launcher.h"
#include "rewind.h"
#include "state.h"

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

/* The front door's ROM check (the kit's playbook/porting.md, "The player's front door"): a dump is
   the cartridge when its bytes are, whatever its name or extension. The forms dumps come in, each
   tried and the one whose SHA-1 is the cartridge's taken: the plain image (.md, .bin, .gen), the
   image after a 512-byte copier header, the Super Magic Drive's interleave (.smd: a 512-byte header,
   then 16 KB blocks, each the odd bytes' 8 KB then the even bytes'; also without the header), and
   the Multi Game Doctor's (.md from some tools: all the odd bytes, then all the even ones). Files
   far from 2 MB are refused before reading. */
#define DK_ROM_MAX_FILE (DK_ROM_SIZE + 0x4000u)

static void smd_deinterleave(const uint8_t *in, size_t n, uint8_t *out)
{
    for (size_t b = 0; b + 0x4000 <= n; b += 0x4000)
        for (size_t i = 0; i < 0x2000; i++) {
            out[b + 2 * i + 1] = in[b + i];
            out[b + 2 * i] = in[b + 0x2000 + i];
        }
}

static void mgd_deinterleave(const uint8_t *in, size_t n, uint8_t *out)
{
    size_t h = n / 2;
    for (size_t i = 0; i < h; i++) {
        out[2 * i + 1] = in[i];
        out[2 * i] = in[h + i];
    }
}

/* "" if PATH holds the cartridge (its bytes then in out, DK_ROM_SIZE of them; *form says which form),
   else why not, with "SHA-1" in it for a file that is read and is not the game */
static std::string rom_check(const char *path, std::vector<uint8_t> *out, const char **form = nullptr)
{
    FILE *f = fopen(path, "rb");
    if (!f) return std::string("cannot open the ROM ") + path;
    std::vector<uint8_t> file;
    uint8_t buf[65536];
    size_t n;
    while ((n = fread(buf, 1, sizeof buf, f)) > 0) {
        file.insert(file.end(), buf, buf + n);
        if (file.size() > DK_ROM_MAX_FILE) break;
    }
    fclose(f);
    char first[41] = "";
    sha1_hex(file.data(), file.size(), first);
    std::vector<uint8_t> img(DK_ROM_SIZE);
    auto is_game = [&](const uint8_t *p) {
        char got[41];
        sha1_hex(p, DK_ROM_SIZE, got);
        return !strcmp(got, DK_ROM_SHA1);
    };
    const char *how = nullptr;
    if (file.size() == DK_ROM_SIZE && is_game(file.data())) {
        how = "plain";
        memcpy(img.data(), file.data(), DK_ROM_SIZE);
    } else if (file.size() == DK_ROM_SIZE + 512u && is_game(file.data() + 512)) {
        how = "copier header";
        memcpy(img.data(), file.data() + 512, DK_ROM_SIZE);
    } else if (file.size() == DK_ROM_SIZE + 512u && (smd_deinterleave(file.data() + 512, DK_ROM_SIZE, img.data()), is_game(img.data()))) {
        how = "SMD interleaved";
    } else if (file.size() == DK_ROM_SIZE && (smd_deinterleave(file.data(), DK_ROM_SIZE, img.data()), is_game(img.data()))) {
        how = "SMD interleaved, no header";
    } else if (file.size() == DK_ROM_SIZE && (mgd_deinterleave(file.data(), DK_ROM_SIZE, img.data()), is_game(img.data()))) {
        how = "MGD interleaved";
    }
    if (!how) {
        char why[600];
        snprintf(why, sizeof why, "%s is not %s (USA): its SHA-1 is %s (%zu%s bytes), not %s, in any form a dump "
                                  "comes in (plain, copier header, SMD or MGD interleave). A zipped ROM must be "
                                  "unzipped first; another version of the cartridge is refused. Use your own dump of "
                                  "the cartridge.", path, DK_TITLE, first, file.size(),
                 file.size() > DK_ROM_MAX_FILE ? "+" : "", DK_ROM_SHA1);
        return why;
    }
    if (form) *form = how;
    if (out) out->swap(img);
    return "";
}

std::string rom_check_path(const char *path) { return rom_check(path, nullptr); }   /* the launcher's */

static int s_played_messages;   /* played (no --benchmark): a refusal is also a message box */

static void load_rom(const char *path)
{
    std::vector<uint8_t> img;
    const char *form = "plain";
    std::string why = rom_check(path, &img, &form);
    if (!why.empty()) {
        fprintf(stderr, "[native] %s: refusing to run.\n", why.c_str());
        fflush(stderr);
        if (s_played_messages) host_message_error(why.c_str());   /* a double-click has no console to read */
        exit(2);
    }
    memset(g_rom, 0, sizeof g_rom);
    memcpy(g_rom, img.data(), DK_ROM_SIZE);
    if (strcmp(form, "plain")) fprintf(stderr, "[native] %s: the cartridge (%s)\n", path, form);
}

/* No ROM on the command line (a double-click): settings.ini's [rom] path (the one chosen last), then
   any file beside the runner that is the game by its bytes, then, played, a file picker (until a file
   that is the game, or cancel); the one picked is remembered in settings.ini. "" if none. */
static std::string find_rom(bool played)
{
    std::string ini = options_ini_rom();
    if (!ini.empty() && rom_check(ini.c_str(), nullptr).empty()) return ini;
    std::error_code ec;
    std::vector<std::string> beside;
    /* the names as fopen takes them (the ANSI code page on Windows, as the exe's folder); a name that
       has no such form is skipped */
    for (const auto &e : std::filesystem::directory_iterator(std::filesystem::path(options_exe_dir()), ec)) {
        std::error_code ec2;
        if (!e.is_regular_file(ec2)) continue;
        uintmax_t sz = e.file_size(ec2);
        if (ec2 || (sz != DK_ROM_SIZE && sz != DK_ROM_SIZE + 512u)) continue;
        try { beside.push_back(e.path().string()); } catch (...) {}
    }
    std::sort(beside.begin(), beside.end());   /* the same choice every time */
    for (const std::string &p : beside)
        if (rom_check(p.c_str(), nullptr).empty()) return p;
    if (!played) return "";
    char path[4096];
    for (;;) {
        if (!host_pick_rom(path, sizeof path)) {
            host_message("The Adventures of Batman and Robin needs your ROM of the cartridge (USA). Put it in the "
                         "game's folder, or start the game with it: drag the ROM onto the program, or run  "
                         "batmanrobin_native path\\to\\rom.md  (Linux: ./batmanrobin path/to/rom.md).");
            return "";
        }
        std::string why = rom_check(path, nullptr);
        if (why.empty()) {
            if (!options_remember_rom(path)) fprintf(stderr, "[native] could not write the ROM's path to settings.ini\n");
            return path;
        }
        host_message_error(why.c_str());   /* and the picker again */
    }
}

/* ======================================================================== */
/* Sync rows (spec.c, DK_NATIVE: from the exporter's image only) */

static void crc_init(void) { (void)crc32_fast_tables(); }

static uint32_t crc32_update(uint32_t c, const uint8_t *p, size_t n) { return crc32_fast_update(c, p, n); }

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

/* `[dkit] enhance {...}`: phase 6's counters, and with quick saves and rewind on (a played run, or
   BR_REWIND_KEYS) their own, "rewind" (rewind.cpp: saves, loads, steps, the ring's size and cost) */
static std::string enhance_line(void)
{
    std::string e = enh_json(1);
    if (rewind_on() && !e.empty() && e.back() == '}')
        e.insert(e.size() - 1, ", \"rewind\": " + rewind_counters_json() + ", \"state_sections\": " + state_sections_json());
    return e;
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
    fprintf(stderr, "[dkit] enhance %s\n", enhance_line().c_str());
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

/* quick saves and rewind (rewind.cpp; the runner's side below play()) */
static int s_played_run;
static std::vector<uint8_t> s_in_script;   /* headless: the script's place the next frame begins with */
static std::vector<uint8_t> s_in_played;   /* played: the frame running's pads, log place, script place */
static int s_replay;                        /* played: the next frame runs with s_replay_pads */
static uint16_t s_replay_pads[2];

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
    if (!ns_load(script) || ns_mutates_state()) {
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
                    "[--branch-plan F] [--options K=V,...] [--record-inputs F] [--present-log F] [--flip N] [--game-rows] [--hash-centre] "
                    "[--render-from F] [--audio-hash] [--suppress-sfx] [--launcher | --no-launcher] [--no-messagebox]\n"
                    "       batmanrobin_native --options --json\n"
                    "  (played without ROM: settings.ini's [rom] path, the ROM beside the program, or a file picker)\n");
}

static int play(void);

int main(int argc, char **argv)
{
    options_init(&argc, argv);   /* settings.ini [options], then --options (exits after --options --json) */
    const char *rom = nullptr, *script = nullptr, *plan = nullptr, *present_log = nullptr, *record = nullptr;
    uint32_t benchmark = 0, hash_frames = 0;
    int no_render = 0;
    int launcher = -1;   /* --launcher 1, --no-launcher 0, else settings.ini's [launcher] show (launcher.cpp) */
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
        else if (!strcmp(a, "--record-inputs") && i + 1 < argc) record = argv[++i];   /* capture.cpp */
        else if (!strcmp(a, "--fresh-evidence")) {}
        else if (!strcmp(a, "--launcher")) launcher = 1;          /* show it, whatever settings.ini says */
        else if (!strcmp(a, "--no-launcher")) launcher = 0;       /* ... or not, this once */
        else if (!strcmp(a, "--no-messagebox")) host_messages_print(1);   /* every message to stderr instead */
        else if (a[0] == '-' && a[1] == '-') fprintf(stderr, "[native] ignoring %s\n", a);
        else if (!rom) rom = a;
        else { usage(); return 2; }
    }
    if (const char *t = getenv("BR_PAD_SELFTEST"); t && *t && strcmp(t, "0")) return host_pad_selftest();
    if (const char *t = getenv("BR_REWIND_SELFTEST"); t && *t && strcmp(t, "0")) return rewind_selftest();   /* rewind.cpp */
    /* The front door (the kit's playbook/porting.md): a played run (no --benchmark) closes the console
       window a double-click opened, says what went wrong in a message box, and, with no ROM given, may
       show the launcher and find the ROM. A harness run passes a ROM and --benchmark: none of this. */
    const int played = !benchmark;
    s_played_messages = played;
    if (played) host_detach_lone_console();
    const char *play_script = getenv("BR_PLAY_SCRIPT");
    const char *no_launcher = getenv("GENESIS_NO_LAUNCHER");
    if (launcher < 0)   /* a double-click: no ROM, no input script, not the kit's harness, not turned off */
        launcher = played && !rom && !script && !(play_script && *play_script) &&
                   !(no_launcher && *no_launcher && strcmp(no_launcher, "0")) &&
                   options_ini_get("launcher", "show", "1") != "0";
    std::string found;
    if (launcher && played) {
        if (!launcher_run(rom ? std::string(rom) : find_rom(false), &found)) return 0;   /* Quit: nothing ran */
        if (const char *d = getenv("BR_LAUNCHER_DRYRUN"); d && *d && strcmp(d, "0")) {   /* the test: the plan */
            printf("[launcher] plan {\"rom\": \"%s\", \"options\": %s}\n", found.c_str(), options_effective_json().c_str());
            fflush(stdout);
            return 0;
        }
        rom = found.c_str();
    }
    if (!rom) {
        found = find_rom(played);
        if (found.empty()) { usage(); return 2; }
        rom = found.c_str();
    }
    load_rom(rom);   /* refuses anything but the cartridge (exit 2) */
    crash_report_install(rom, !benchmark);   /* a stop leaves crash-reports/<time>/ (R27) */
    if (record) {
        if (benchmark) { fprintf(stderr, "--record-inputs is for played sessions (a replay has its script)\n"); return 2; }
        crash_log_record(record);
    }

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
    capture_init();   /* BR_CAPTURE: the pictures to a video (capture.cpp) */
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
    rewind_init(0);   /* on only with BR_REWIND_KEYS (tools/rewindcheck.py) */
    if (rewind_on()) {
        if (plan) { fprintf(stderr, "BR_REWIND_KEYS and --branch-plan: one or the other\n"); return 2; }
        s_in_script.resize(ns_position_save(nullptr));
        ns_position_save(s_in_script.data());
    }
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
        crash_test_frame(f);
        auto tf = timed ? std::chrono::steady_clock::now() : t0;
        nm_run_frame();
        if (g_enh_audio_frame) g_enh_audio_frame(f);   /* the sound side's (--audio-hash, the counters) */
        audio_event_queue_reset();   /* headless: nothing is heard */
        if (s_extra_cols) row_flush(rendered);
        if (timed)
            enh_present_frame(f, s_last_gameplay, rendered,
                              std::chrono::duration<double, std::micro>(std::chrono::steady_clock::now() - tf).count());
        if (rendered) {   /* BR_CAPTURE (capture.cpp): nothing without it */
            int cs, cw, ch;
            const uint32_t *cfb = nm_framebuffer(&cs, &cw, &ch);
            capture_frame(cfb, cs, cw, ch);
        }
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
        /* quick saves and rewind driven by BR_REWIND_KEYS (rewind.cpp's tests): a load moves the frame;
           a held picture has nothing to show headless, the steps still count */
        if (rewind_on()) {
            int r = rewind_after_frame(true);
            while (r == RW_HOLD) r = rewind_after_frame(false);
            if (rewind_loaded()) s_last_frame = s_ticks = frame_num = (uint32_t)nm_frame_no();
            s_in_script.resize(ns_position_save(nullptr));
            ns_position_save(s_in_script.data());   /* where the next frame's input is (main_frame_input) */
        }
    }
    double secs = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
    crash_log_finish();
    fprintf(stderr, "[DONE] %u frames completed\n", frame_num);
    printf("GENESISRECOMP_BENCHMARK {\"game\":\"BATMANROBIN\",\"frames\":%u,\"seconds\":%.9f,\"fps\":%.3f,"
           "\"ms_per_frame\":%.6f,\"requested_frames\":%u,\"complete\":%s,\"runner\":\"native\"}\n",
           frame_num, secs, secs > 0 ? frame_num / secs : 0.0, frame_num ? secs * 1000.0 / frame_num : 0.0, benchmark,
           frame_num == benchmark ? "true" : "false");
    fflush(stdout);
    return 0;
}

/* ======================================================================== */
/* Quick saves and rewind (rewind.cpp, R33): the runner's side. The input a frame began with is kept
   with it in the rewind ring and given back for a replay: played, the pads, where the session's input
   log stood (crash_report.h: the log follows the timeline kept) and, with BR_PLAY_SCRIPT, where the
   script stood; headless (BR_REWIND_KEYS), where the input script stood. Beside a quick save: the whole
   log (played), the script's place. */

static void put_part(std::vector<uint8_t> &o, const std::vector<uint8_t> &p)
{
    uint32_t n = (uint32_t)p.size();
    o.insert(o.end(), (const uint8_t *)&n, (const uint8_t *)&n + sizeof n);
    o.insert(o.end(), p.begin(), p.end());
}

static bool get_part(const std::vector<uint8_t> &in, size_t *at, const uint8_t **p, size_t *n)
{
    uint32_t k;
    if (in.size() - *at < sizeof k) return false;
    memcpy(&k, in.data() + *at, sizeof k);
    *at += sizeof k;
    if (in.size() - *at < k) return false;
    *p = in.data() + *at;
    *n = k;
    *at += k;
    return true;
}

static std::vector<uint8_t> script_place(void)
{
    std::vector<uint8_t> v(ns_active() ? ns_position_save(nullptr) : 0);
    if (!v.empty()) ns_position_save(v.data());
    return v;
}

void main_frame_input(std::vector<uint8_t> &out) { out = s_played_run ? s_in_played : s_in_script; }

void main_replay_input(const std::vector<uint8_t> &in)
{
    if (!s_played_run) {
        if (!ns_position_load(in.data(), in.size())) fprintf(stderr, "[rewind] the script's place did not load\n");
        return;
    }
    size_t at = 0, n[3];
    const uint8_t *p[3];
    for (int i = 0; i < 3; i++)
        if (!get_part(in, &at, &p[i], &n[i])) return;
    if (n[0] == sizeof s_replay_pads) { memcpy(s_replay_pads, p[0], sizeof s_replay_pads); s_replay = 1; }
    crash_log_back_to(p[1], n[1]);
    if (n[2]) ns_position_load(p[2], n[2]);
}

void main_timeline_save(std::vector<uint8_t> &out)
{
    out.clear();
    if (!s_played_run) { out = script_place(); return; }
    std::vector<uint8_t> log;
    crash_log_copy(log);
    put_part(out, log);
    put_part(out, script_place());
}

void main_timeline_load(const std::vector<uint8_t> &in)
{
    if (!s_played_run) {
        if (!in.empty()) ns_position_load(in.data(), in.size());
        return;
    }
    size_t at = 0, n[2];
    const uint8_t *p[2];
    for (int i = 0; i < 2; i++)
        if (!get_part(in, &at, &p[i], &n[i])) return;
    crash_log_paste(std::vector<uint8_t>(p[0], p[0] + n[0]));
    if (n[1]) ns_position_load(p[1], n[1]);
}

/* a played run's exit line: the options and the counters, rewind's with them (a player's log) */
static void report_played(void)
{
    fprintf(stderr, "[dkit] options %s\n", options_effective_json().c_str());
    fprintf(stderr, "[dkit] enhance %s\n", enhance_line().c_str());
    fflush(stderr);
}

/* ======================================================================== */
/* Played: the window, the controllers, the sound; one host frame per displayed frame */

static int play(void)
{
    if (!host_open(DK_TITLE)) return 1;
    nm_set_play(1);
    nm_set_render(1);
    hf_init();
    s_played_run = 1;
    rewind_init(1);   /* quick saves and rewind: F5-F7, Rewind held */
    atexit(report_played);
    uint64_t frame = 0, clock_from = 0;
    int clock_on = 0;
    static std::vector<uint32_t> between((size_t)512 * 480);
    const double T_US = 896040.0 * 1e6 / 53693175.0;   /* one NTSC frame: the present clock's unit */
    /* BR_PLAY_SCRIPT=F: the pads from an input script instead of the controllers, one script frame
       per played frame (the crash report's log is checked against it: R27); its EXIT ends the run */
    const char *play_script = getenv("BR_PLAY_SCRIPT");
    if (play_script) {
        if (!ns_load(play_script)) return 2;
        ns_tick(0);
    }
    /* the picture to the window: high_fps's clock counts host frames (a rewind step or a held picture is
       one too), so a load moves nothing on it; the interpolator's previous frame goes with a load */
    auto show = [&](const uint32_t *fb, int stride, int w, int h) {
        if (g_opt.high_fps > 0) {
            /* high_fps (R3, R28): R_k is made; the span (k-1)T..kT in even parts (enh_between_times)
               shows it moved back toward R_(k-1) (present.cpp), then R_k at kT (host_present_at
               waits for each time) */
            if (!clock_on) { clock_on = 1; clock_from = frame - 1; host_present_clock_reset(); }
            uint64_t k = frame - clock_from;
            if (k >= 2) {
                double t0 = (double)(k - 1) * T_US, t1 = (double)k * T_US, ts[64];
                int nb = enh_between_times(t0, t1, g_opt.high_fps, ts, 64);
                for (int j = 0; j < nb; j++)
                    if (hf_interpolate(k, (ts[j] - t0) / T_US, between.data(), 512, w, h))
                        host_present_at(between.data(), 512, w, h, ts[j]);
            }
            host_present_at(fb, stride, w, h, (double)k * T_US);
        } else {
            if (clock_on) { clock_on = 0; host_present_clock_reset(); }
            host_present(fb, stride, w, h);
        }
        hf_frame_done();
    };
    for (;;) {
        uint16_t pads[2];
        host_input(pads);   /* the pads are read when the 60 Hz build reads them: right after the last frame */
        if (play_script) pads[0] = frame_pad(0), pads[1] = frame_pad(1);
        uint16_t q1 = s_types[0] == 1 ? pads[0] : (uint16_t)(pads[0] & 0xFFu);
        uint16_t q2 = s_types[1] == 1 ? pads[1] : (uint16_t)(pads[1] & 0xFFu);
        if (s_replay) {   /* a rewind step: the frame's own pads again (the keys were still read) */
            q1 = s_replay_pads[0], q2 = s_replay_pads[1];
            s_replay = 0;
        }
        if (rewind_on()) {   /* what this frame begins with (main_frame_input) */
            std::vector<uint8_t> log;
            crash_log_position(log);
            uint16_t q[2] = { q1, q2 };
            s_in_played.clear();
            put_part(s_in_played, std::vector<uint8_t>((const uint8_t *)q, (const uint8_t *)q + sizeof q));
            put_part(s_in_played, log);
            put_part(s_in_played, play_script ? script_place() : std::vector<uint8_t>());
        }
        crash_test_frame(frame + 1);
        crash_log_frame(q1, q2);   /* the session's inputs, for a report (crash_report.h) */
        nm_set_pads(q1, q2);
        nm_run_frame();
        if (play_script) ns_tick(nm_frame_no());   /* the frame run (a load moves it: rewind.cpp) */
        if (nm_game_stopped()) {
            fprintf(stderr, "[native] the game stopped\n");
            crash_report_write("the game stopped (its machine ended the run)");
            return 1;
        }
        host_audio(++frame, rewind_audio());   /* a rewind step's frame: backwards or silent ([rewind] sound) */
        int stride, w, h;
        const uint32_t *fb = nm_framebuffer(&stride, &w, &h);
        capture_frame(fb, stride, w, h);   /* BR_CAPTURE (capture.cpp) */
        show(fb, stride, w, h);
        if (play_script && ns_should_exit()) {
            fprintf(stderr, "[input_script] exiting per script directive (code=%d)\n", ns_exit_code());
            exit(ns_exit_code());
        }
        /* quick saves and rewind: held at the ring's oldest frame, the picture stays and the game waits */
        for (int r = rewind_after_frame(true); r == RW_HOLD; r = rewind_after_frame(false)) {
            host_input(pads);
            host_audio_silence();
            ++frame;
            fb = nm_framebuffer(&stride, &w, &h);
            show(fb, stride, w, h);
        }
    }
}
