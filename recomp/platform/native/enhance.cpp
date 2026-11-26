/* enhance.cpp - phase 6's counters, the present clock and the game's state (enhance.h; the
 * contract in docs/NOTES.md "Phase 6: the contract").
 *
 *   the counters      g_enh, printed at exit as `[dkit] enhance {...}` (runner.cpp), each branch of
 *                     a branch plan its own
 *   the present clock --present-log OUT (present.csv) and high_fps: runner frame k's picture R_k is
 *                     shown at k x T (T one NTSC frame, as the 60 Hz build shows it); with
 *                     high_fps = H the span (k-1) x T .. k x T in n = H x T rounded even parts
 *                     (R28: enh_between_times), its n - 1 inner times
 *                     showing frames interpolated from R_(k-1) toward R_k (the interpolator,
 *                     enh_set_interpolator; without one, R_(k-1) again). R_k is made before them:
 *                     no added latency, the frames in between only show earlier what R_k shows.
 *   the game's state  --game-rows: game_crc32 (view_masks.h) and gameplay beside each sync row
 *   the centre        --hash-centre: fb_centre, the centre 320 columns of each runner frame's picture
 *   the render timer  one sample per displayed frame; p99 in the present counters
 */
#include <algorithm>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

extern "C" {
#include "video/genesis_machine.h"
}
#include "enhance.h"
#include "crc32_fast.h"
#include "runtime/dk_image.h"
#include "native_machine.h"
#include "options.h"
#include "sound2.h"
#include "view_masks.h"
#include "widescreen.h"

EnhCounters g_enh;
EnhFlags g_enh_flags = { 0, 0, 0, 0, 0, -1, 0 };
int g_enh_centre_x0 = -1;
int g_enh_centre_skip = 0;
EnhRect g_enh_centre_rects[ENH_CENTRE_RECTS];
int g_enh_centre_nrects;
void (*g_enh_audio_frame)(uint64_t frame);

static EnhInterpolator s_interp;
static std::vector<uint32_t> s_render_us;

static double enh_now_us(void)
{
    return std::chrono::duration<double, std::micro>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

/* ---- the game's flags (68000 byte order) ---- */

#define RAM_SECTION     0xF602u   /* g_SectionId (word) */
#define RAM_FADE_TARGET 0xF702u   /* g_FadeTarget (byte) */
#define RAM_FADE_LEVEL  0xF703u   /* g_FadeLevel (byte) */

extern "C" uint16_t enh_section(const uint8_t *ram)
{
    return (uint16_t)(ram[RAM_SECTION] << 8 | ram[RAM_SECTION + 1]);
}

extern "C" int enh_gameplay(const uint8_t *ram)
{
    uint16_t s = enh_section(ram);
    return s >= 0x1100u && s < 0x5000u && !ram[RAM_FADE_LEVEL] && !ram[RAM_FADE_TARGET];
}

/* ---- helpers for the pieces ---- */

extern "C" void enh_ws_screen(const char *id, const char *mode)
{
    EnhWs &w = g_enh.ws;
    for (int i = 0; i < w.nscreens; i++)
        if (!strcmp(w.screens[i].id, id)) {
            if (strcmp(w.screens[i].mode, mode)) snprintf(w.screens[i].mode, sizeof w.screens[i].mode, "mixed");
            w.screens[i].frames++;
            return;
        }
    if (w.nscreens >= ENH_SCREENS) return;
    EnhScreen &s = w.screens[w.nscreens++];
    snprintf(s.id, sizeof s.id, "%s", id);
    snprintf(s.mode, sizeof s.mode, "%s", mode);
    s.frames = 1;
}

static EnhSectionCount *scroll_entry(uint16_t section)
{
    EnhPresent &p = g_enh.present;
    int i = 0;
    while (i < p.nscroll && p.scroll[i].section != section) i++;
    if (i == p.nscroll) {
        if (p.nscroll >= ENH_SECTIONS) return nullptr;
        p.scroll[p.nscroll++] = { section, 0, 0, 0 };
    }
    return &p.scroll[i];
}

extern "C" void enh_scroll_frame(uint16_t section, int interpolated)
{
    if (EnhSectionCount *s = scroll_entry(section)) {
        s->frames++;
        s->interpolated += interpolated ? 1 : 0;
    }
}

/* a section reached with per-line scroll that never moves (the Freeze fight's $4401 without its
   shake) stays visible in the report with frames 0 */
extern "C" void enh_scroll_static(uint16_t section)
{
    if (EnhSectionCount *s = scroll_entry(section)) s->static_frames++;
}

extern "C" void enh_set_interpolator(EnhInterpolator f)
{
    s_interp = f;
    g_enh.present.interpolator = f != nullptr;
}

extern "C" void enh_render_sample(double us) { s_render_us.push_back((uint32_t)(us < 0 ? 0 : us)); }

static uint32_t render_p99(void)
{
    if (s_render_us.empty()) return 0;
    std::vector<uint32_t> v = s_render_us;
    size_t i = (v.size() * 99) / 100;
    if (i >= v.size()) i = v.size() - 1;
    std::nth_element(v.begin(), v.begin() + (long)i, v.end());
    return v[i];
}

/* ---- the game's state (--game-rows) ---- */

static uint32_t crc_update(uint32_t c, const uint8_t *p, size_t n) { return crc32_fast_update(c, p, n); }

static void zero(uint8_t *mem, uint32_t size, uint32_t lo, uint32_t n)
{
    if (lo >= size) return;
    if (n > size - lo) n = size - lo;
    memset(mem + lo, 0, n);
}

/* ram_mask: the sync rows' RAM mask (1 = masked), 64 KB */
uint32_t enh_game_crc(const DkImage *im, const uint8_t *ram_mask)
{
    static uint8_t ram[DK_IMAGE_RAM_BYTES], vram[DK_IMAGE_VRAM_BYTES];
    for (uint32_t i = 0; i < sizeof ram; i++) ram[i] = ram_mask[i] ? 0 : im->ram[i];
    memcpy(vram, im->vram, sizeof vram);
    const uint8_t *r = g_machine.vdp.reg;
    int h40 = (r[12] & 0x81) != 0;
    static const int dim[4] = { 32, 64, 32, 128 };
    uint32_t plane = (uint32_t)dim[r[16] & 3] * (uint32_t)dim[(r[16] >> 4) & 3] * 2u;
    zero(vram, sizeof vram, (uint32_t)(r[2] & 0x38) << 10, plane);                       /* plane A */
    zero(vram, sizeof vram, (uint32_t)(r[4] & 0x07) << 13, plane);                       /* plane B */
    zero(vram, sizeof vram, (uint32_t)(r[3] & (h40 ? 0x3C : 0x3E)) << 10, h40 ? 0x1000u : 0x800u);   /* window */
    zero(vram, sizeof vram, (uint32_t)(r[5] & (h40 ? 0x7E : 0x7F)) << 9, h40 ? 0x280u : 0x200u);      /* sprites */
    for (const EnhMask *m = k_view_masks; m->what; m++) {
        if (m->hi <= m->lo) continue;
        if (m->space == ENH_RAM) zero(ram, sizeof ram, m->lo, m->hi - m->lo);
        else zero(vram, sizeof vram, m->lo, m->hi - m->lo);
    }
    uint32_t c = 0xFFFFFFFFu;
    c = crc_update(c, ram, sizeof ram);
    c = crc_update(c, vram, sizeof vram);
    c = crc_update(c, im->cram, sizeof im->cram);
    c = crc_update(c, im->vsram, sizeof im->vsram);
    return c ^ 0xFFFFFFFFu;
}

/* ---- picture hashes ---- */

#define FNV_PRIME 0x100000001B3ULL

/* FNV-1a-64 over a pixel's four bytes in memory order (little-endian: the low byte first) */
static inline uint64_t fnv_px(uint64_t x, uint32_t px)
{
    x ^= px & 0xFFu;         x *= FNV_PRIME;
    x ^= (px >> 8) & 0xFFu;  x *= FNV_PRIME;
    x ^= (px >> 16) & 0xFFu; x *= FNV_PRIME;
    x ^= px >> 24;           x *= FNV_PRIME;
    return x;
}

static uint64_t fnv(const uint32_t *fb, int stride, int x0, int w, int h)
{
    uint64_t x = 0xCBF29CE484222325ULL;
    for (int y = 0; y < h; y++) {
        const uint32_t *row = fb + (size_t)y * (size_t)stride + x0;
        for (int i = 0; i < w; i++) x = fnv_px(x, row[i]);
    }
    return x;
}

uint64_t enh_fb_hash(const uint32_t *fb, int stride, int w, int h) { return fnv(fb, stride, 0, w, h); }

uint64_t enh_centre_hash(void)
{
    int stride, w, h;
    const uint32_t *fb = nm_framebuffer(&stride, &w, &h);
    int sw = gvdp_screen_width(&g_machine.vdp);
    int x0 = g_enh_centre_x0 >= 0 ? g_enh_centre_x0 : (w > sw ? (w - sw) / 2 : 0);
    int skip = g_enh_centre_skip > 0 && g_enh_centre_skip < sw ? g_enh_centre_skip : 0;
    if (x0 + sw > w) return 0;
    if (!g_enh_centre_nrects) return fnv(fb, stride, x0 + skip, sw - skip, h);
    /* the listed rectangles (widescreen.cpp: the HUD where either view draws it) hash as zero */
    /* each row's covered columns once (a rect test per pixel was ~1 ms of every runner frame) */
    static uint8_t cover[1024];
    if (sw > (int)sizeof cover) return 0;
    uint64_t x = 0xCBF29CE484222325ULL;
    for (int y = 0; y < h; y++) {
        const uint32_t *row = fb + (size_t)y * (size_t)stride + x0;
        int any = 0;
        for (int k = 0; k < g_enh_centre_nrects; k++) {
            const EnhRect &r = g_enh_centre_rects[k];
            if (y < r.y0 || y >= r.y1) continue;
            int a = r.x0 > skip ? r.x0 : skip, b = r.x1 < sw ? r.x1 : sw;
            if (a >= b) continue;
            if (!any) { memset(cover, 0, (size_t)sw); any = 1; }
            memset(cover + a, 1, (size_t)(b - a));
        }
        if (!any)
            for (int c = skip; c < sw; c++) x = fnv_px(x, row[c]);
        else
            for (int c = skip; c < sw; c++) x = fnv_px(x, cover[c] ? 0u : row[c]);
    }
    return x;
}

/* diagnosing (BR_CENTRE_SHOT=DIR, BR_CENTRE_SHOT_SYNC=a,b,...): the frame of each listed sync row as
   a PPM, with the centre's x0, skip and rectangles beside it (what --hash-centre hashed) */
void enh_centre_shot(uint64_t sync)
{
    static const char *dir = getenv("BR_CENTRE_SHOT");
    static const char *list = getenv("BR_CENTRE_SHOT_SYNC");
    if (!dir || !*dir || !list) return;
    int hit = 0;
    for (const char *p = list; *p;) {
        char *e;
        unsigned long long n = strtoull(p, &e, 10);
        if (e == p) break;
        if (n == sync) hit = 1;
        p = *e ? e + 1 : e;
    }
    if (!hit) return;
    int stride, w, h;
    const uint32_t *fb = nm_framebuffer(&stride, &w, &h);
    int sw = gvdp_screen_width(&g_machine.vdp);
    int x0 = g_enh_centre_x0 >= 0 ? g_enh_centre_x0 : (w > sw ? (w - sw) / 2 : 0);
    char path[700];
    snprintf(path, sizeof path, "%s/c%06llu.ppm", dir, (unsigned long long)sync);
    FILE *f = fopen(path, "wb");
    if (!f) return;
    fprintf(f, "P6\n%d %d\n255\n", w, h);
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            uint32_t px = fb[(size_t)y * (size_t)stride + x];
            unsigned char rgb[3] = {(unsigned char)(px >> 16), (unsigned char)(px >> 8), (unsigned char)px};
            fwrite(rgb, 1, 3, f);
        }
    fclose(f);
    snprintf(path, sizeof path, "%s/c%06llu.txt", dir, (unsigned long long)sync);
    f = fopen(path, "w");
    if (!f) return;
    fprintf(f, "w %d h %d sw %d x0 %d skip %d nrects %d\n", w, h, sw, x0, g_enh_centre_skip, g_enh_centre_nrects);
    for (int k = 0; k < g_enh_centre_nrects; k++)
        fprintf(f, "rect %d %d %d %d\n", g_enh_centre_rects[k].x0, g_enh_centre_rects[k].y0,
                g_enh_centre_rects[k].x1, g_enh_centre_rects[k].y1);
    fclose(f);
}

/* ---- the present clock ---- */

static const double T_US = 896040.0 * 1e6 / 53693175.0;   /* one NTSC frame (262 x 3420 master clocks) */
static FILE *s_log;
static std::vector<uint32_t> s_prev;   /* R_(k-1), for the frames in between (no interpolator) */
static int s_prev_w, s_prev_h, s_prev_ok;
static std::vector<uint32_t> s_tmp;

void enh_present_init(const char *path)
{
    if (path) {
        s_log = fopen(path, "w");
        if (!s_log) { fprintf(stderr, "[native] cannot write the present log %s\n", path); exit(2); }
        setvbuf(s_log, nullptr, _IOFBF, 1 << 20);
        fprintf(s_log, "sync,t_us,whole,gameplay,fb_fnv1a64\n");
    }
    if (s_log || g_opt.high_fps) {
        g_enh.present.active = 1;
        g_enh.present.hz = g_opt.high_fps;
    }
}

static void row(uint64_t k, double t, int whole, int gameplay, uint64_t hash, int hashed)
{
    g_enh.present.frames++;
    g_enh.present.whole += whole ? 1u : 0u;
    if (s_log) {
        if (hashed) fprintf(s_log, "%llu,%.1f,%d,%d,%016llX\n", (unsigned long long)k, t, whole, gameplay, (unsigned long long)hash);
        else fprintf(s_log, "%llu,%.1f,%d,%d,\n", (unsigned long long)k, t, whole, gameplay);
    }
}

/* ---- R29: the renderer's sprite-table copy against VRAM ---- */

static int s_sat_on, s_sat_bad;

void enh_sat_begin(void) { s_sat_on = 1; s_sat_bad = 0; }

void enh_sat_line(const GVDP *v)
{
    if (!s_sat_on || s_sat_bad) return;
    const int h40 = gvdp_screen_width(v) == 320;
    const uint16_t base = h40 ? 0xFC00 : 0xFE00, sb = (uint16_t)((v->reg[5] << 9) & base);
    for (int s = 0; s < (h40 ? 80 : 64); s++)
        for (int i = 0; i < 4; i++)   /* y, size, link: what the renderer reads from the copy */
            if (v->sat[s * 8 + i] != v->vram[(uint16_t)(sb + s * 8 + i)]) { s_sat_bad = 1; return; }
}

void enh_sat_end(void)
{
    if (!s_sat_on) return;
    s_sat_on = 0;
    g_enh.sat_checks++;
    if (s_sat_bad) g_enh.sat_cache_mismatch++;
}

int enh_sat_selftest(void)
{
    static int on = -1;
    if (on < 0) { const char *e = getenv("BR_SAT_SELFTEST"); on = e && *e == '1'; }
    return on;
}

int enh_between_times(double t0, double t1, int hz, double *ts, int max)
{
    if (hz <= 0 || t1 <= t0) return 0;
    int n = (int)((t1 - t0) * hz / 1e6 + 0.5);
    if (n < 1) n = 1;
    int m = 0;
    for (int j = 1; j < n && m < max; j++) ts[m++] = t0 + j * (t1 - t0) / n;
    return m;
}

/* After runner frame k (its picture R_k rendered when `rendered`, its logic done): the frames in
   between R_(k-1) and R_k (the span's even parts, enh_between_times), then R_k at k x T. frame_us:
   the runner frame's time (R_k's sample). */
void enh_present_frame(uint64_t k, int gameplay, int rendered, double frame_us)
{
    if (!g_enh.present.active) return;
    int stride, w, h;
    const uint32_t *fb = nm_framebuffer(&stride, &w, &h);
    int hz = g_opt.high_fps;
    if (hz > 0 && k >= 2) {
        double t0 = (double)(k - 1) * T_US, t1 = (double)k * T_US, ts[64];
        int nb = enh_between_times(t0, t1, hz, ts, 64);
        for (int j = 0; j < nb; j++) {
            double t = ts[j];
            int made = 0;
            uint64_t hash = 0;
            if (rendered) {
                if ((int)s_tmp.size() < 512 * 480) s_tmp.resize(512 * 480);
                double t_a = enh_now_us();
                if (s_interp) made = s_interp(k, (t - t0) / T_US, s_tmp.data(), 512, w, h);
                enh_render_sample(enh_now_us() - t_a);
                if (!s_log) {}   /* the hash is the log's only reader (the corpus passes keep the time) */
                else if (made) hash = fnv(s_tmp.data(), 512, 0, w, h);
                else if (s_prev_ok && s_prev_w == w && s_prev_h == h) hash = fnv(s_prev.data(), 512, 0, w, h);
                else hash = fnv(fb, stride, 0, w, h);
            }
            row(k, t, 0, gameplay, hash, rendered);
        }
    }
    enh_render_sample(frame_us);
    /* E4's negative control (HF_PLANT_LATE=K, high_fps on): runner frame K's whole row shows R_(K-1)
       (R_K shown late); the kit's whole-frame check must fail at sync K */
    static long long late = -2;
    if (late == -2) { const char *e = getenv("HF_PLANT_LATE"); late = e && *e ? atoll(e) : -1; }
    if (hz > 0 && (long long)k == late && rendered && s_log && s_prev_ok && s_prev_w == w && s_prev_h == h) {
        row(k, (double)k * T_US, 1, gameplay, fnv(s_prev.data(), 512, 0, w, h), rendered);
        fprintf(stderr, "[hf] HF_PLANT_LATE: frame %lld's whole row shows R_(k-1)\n", late);
    } else {
        row(k, (double)k * T_US, 1, gameplay, rendered && s_log ? fnv(fb, stride, 0, w, h) : 0, rendered);
    }
    if (hz > 0 && rendered && s_log) {   /* R_(k-1) for the log's repeated frames */
        if ((int)s_prev.size() < 512 * 480) s_prev.resize(512 * 480);
        for (int y = 0; y < h; y++) memcpy(s_prev.data() + (size_t)y * 512, fb + (size_t)y * stride, (size_t)w * 4);
        s_prev_w = w;
        s_prev_h = h;
        s_prev_ok = 1;
    }
}

void enh_present_close(void)
{
    if (s_log) { fclose(s_log); s_log = nullptr; }
}

/* ---- the report ---- */

static std::string u64(uint64_t v) { return std::to_string((unsigned long long)v); }

static std::string arr6(const uint64_t *a)
{
    std::string o = "[";
    for (int i = 0; i < ENH_FM; i++) o += (i ? ", " : "") + u64(a[i]);
    return o + "]";
}

std::string enh_json(int with_render)
{
    const EnhCounters &e = g_enh;
    char b[64];
    std::string o = "{\"gameplay_frames\": " + u64(e.gameplay_frames) + ", \"sat_checks\": " + u64(e.sat_checks) +
                    ", \"sat_cache_mismatch\": " + u64(e.sat_cache_mismatch);
    o += ", \"present\": ";
    if (e.present.active) {
        const EnhPresent &p = e.present;
        o += "{\"hz\": " + std::to_string(p.hz) + ", \"frames\": " + u64(p.frames) + ", \"whole\": " + u64(p.whole) +
             ", \"interpolator\": " + (p.interpolator ? "true" : "false") + ", \"sprites\": " + u64(p.sprites) +
             ", \"sprites_matched\": " + u64(p.sprites_matched) + ", \"render_us_p99\": " +
             (with_render ? std::to_string(render_p99()) : std::string("null")) +
             ", \"render_samples\": " + u64(with_render ? s_render_us.size() : 0) + ", \"scroll\": {";
        for (int i = 0; i < p.nscroll; i++) {
            snprintf(b, sizeof b, "%s\"%04X\": ", i ? ", " : "", (unsigned)p.scroll[i].section);
            o += b + std::string("{\"frames\": ") + u64(p.scroll[i].frames) + ", \"interpolated\": " +
                 u64(p.scroll[i].interpolated) + ", \"static_frames\": " + u64(p.scroll[i].static_frames) + "}";
        }
        o += "}}";
    } else o += "null";
    o += ", \"ws\": ";
    if (e.ws.active) {
        const EnhWs &w = e.ws;
        o += "{\"gameplay_frames\": " + u64(w.gameplay_frames) + ", \"terrain_checks\": " + u64(w.terrain_checks) +
             ", \"terrain_errors\": " + u64(w.terrain_errors) + ", \"streamed_frames\": " + u64(w.streamed_frames) +
             ", \"streamed_lines\": " + u64(w.streamed_lines) + ", \"streamed_lines_apart\": " +
             u64(w.streamed_lines_apart) + ", \"streamed_cells_left\": " +
             u64(w.streamed_cells_left) + ", \"streamed_cells_right\": " + u64(w.streamed_cells_right) +
             ", \"sprites_lost\": " + u64(w.sprites_lost) +
             ", \"hud_misplaced\": " + u64(w.hud_misplaced) + ", \"bars_unneeded\": " + u64(w.bars_unneeded) +
             ", \"edge_clamped_columns\": " + u64(w.edge_clamped_columns) + ", \"centre_mismatch\": " +
             u64(w.centre_mismatch) + ", \"host_sprites_checked\": " + u64(w.host_sprites_checked) +
             ", \"host_sprites_misplaced\": " + u64(w.host_sprites_misplaced) + ", \"host_shift_frames\": " +
             u64(w.host_shift_frames) + ", \"screens\": {";
        for (int i = 0; i < w.nscreens; i++)
            o += std::string(i ? ", " : "") + "\"" + w.screens[i].id + "\": \"" + w.screens[i].mode + "\"";
        o += "}, \"screen_frames\": {";
        for (int i = 0; i < w.nscreens; i++)
            o += std::string(i ? ", " : "") + "\"" + w.screens[i].id + "\": " + u64(w.screens[i].frames);
        o += "}, " + ws_json_extra() + "}";   /* widescreen.cpp: the first error of each check */
    } else o += "null";
    o += ", \"sprites\": ";
    if (e.sprites.active) {
        const EnhSprites &s = e.sprites;
        o += "{\"line_drops\": " + u64(s.line_drops) + ", \"line_drop_frames\": " + u64(s.line_drop_frames) +
             ", \"list_drops\": " + u64(s.list_drops) + ", \"list_drop_frames\": " + u64(s.list_drop_frames) +
             ", \"x0_masked_lines\": " + u64(s.x0_masked_lines) + ", \"cover_frames\": " + u64(s.cover_frames) + "}";
    } else o += "null";
    o += ", \"audio\": ";
    if (e.audio.active) {
        const EnhAudio &a = e.audio;
        uint64_t in_flight, age;
        snd_fx_in_flight(&in_flight, &age);   /* now: the run's or the branch's end */
        snprintf(b, sizeof b, "\"%016llX\"", (unsigned long long)a.pcm_fnv1a64);
        o += "{\"keyons_by_channel\": " + arr6(a.keyons_by_channel) + ", \"music_machine\": " +
             (a.music_machine ? "true" : "false") + ", \"music_keyons_by_channel\": " +
             (a.music_machine ? arr6(a.music_keyons_by_channel) : std::string("null")) +
             ", \"notes_lost\": " + u64(a.notes_lost) + ", \"sfx_sent\": " + u64(a.sfx_sent) + ", \"sfx_started\": " +
             u64(a.sfx_started) + ", \"sfx_overwritten\": " + u64(a.sfx_overwritten) + ", \"sfx_in_flight\": " +
             u64(in_flight) + ", \"sfx_in_flight_age\": " + u64(age) + ", \"sfx_start_latency_max\": " +
             u64(a.sfx_start_latency_max) + ", \"machine2_reads\": " + u64(a.machine2_reads) + ", \"fade_pause_mismatch\": " +
             u64(a.fade_pause_mismatch) + ", \"audible_outside_effect\": " + u64(a.audible_outside_effect) +
             ", \"pcm_hashed\": " + (a.pcm_hashed ? "true" : "false") +
             ", \"pcm_fnv1a64\": " + (a.pcm_hashed ? std::string(b) : std::string("null")) + ", \"pcm_samples\": " +
             u64(a.pcm_samples) + "}";
    } else o += "null";
    return o + "}";
}

/* ---- branch plans: each branch counts from zero; the carrier's counts are put back after it ---- */

static EnhCounters s_carrier;

static void reset_counts(void)
{
    EnhCounters keep = g_enh;
    memset(&g_enh, 0, sizeof g_enh);
    g_enh.present.active = keep.present.active;
    g_enh.present.hz = keep.present.hz;
    g_enh.present.interpolator = keep.present.interpolator;
    g_enh.ws.active = keep.ws.active;
    g_enh.sprites.active = keep.sprites.active;
    g_enh.audio.active = keep.audio.active;
    g_enh.audio.music_machine = keep.audio.music_machine;
    g_enh.audio.pcm_hashed = keep.audio.pcm_hashed;
}

void enh_branch_begin(void)
{
    s_carrier = g_enh;
    reset_counts();
}

std::string enh_branch_end(void)
{
    std::string j = enh_json(0);
    g_enh = s_carrier;
    return j;
}
