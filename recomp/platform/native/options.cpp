/* options.cpp - the native runner's phase-6 options (docs/phases/P6-enhance.md R1, E2): each
 * enhancement is a runtime option of the one release runner, at its cartridge setting by default,
 * except music_voices (R13: on by default; sound only).
 *
 *   --options --json     print every option (default, values, cartridge setting, the effective
 *                        value) and exit 0; no ROM needed
 *   --options K=V,K=V    set them for this run (any run: played, traced, benchmark)
 *   settings.ini         beside the runner: [options] K=V lines set them too; the command line wins
 *
 * The game's C reads them through g_opt (options.h), in native-only branches; with every option at
 * its cartridge setting no option code runs (music_voices is the platform's sound only).
 *
 * The settings file's other sections are kept for the front door: [rom] path (the ROM chosen last),
 * [launcher] show, [player1] / [player2] (controls.cpp); options_ini_write rewrites one section and
 * keeps the rest (the kit's [input.pN] pad types). The launcher shows the table (options_count ...).
 *
 * Adapted from the X-Men 2 port's platform/native/options.cpp (the same kit).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "options.h"
#include "host_sdl.h"
#include "crash_report.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif

NativeOptions g_opt = {
    /* widescreen */ 0, /* widescreen_gameplay */ 0, /* high_fps */ 0, /* unlimited_sprites */ 0,
    /* music_voices */ 1, /* fm_ladder */ 1, /* fm_filter */ 0, /* weapon_level */ 0, /* start_section */ 0,
};

struct OptDef {
    const char *name;
    const char *values[32];   /* the first is the default; null-terminated */
    const char *cartridge;    /* the original's setting: what every check against phase 5 runs with */
    const char *what;
    std::string value;
};

static OptDef s_defs[] = {
    { "widescreen", { "0", "16:9", "16:9-gameplay", nullptr }, "0",
      "the 16:9 view (R4, R5, R23): 16:9 = the game plays exactly as in 4:3; 16:9-gameplay = objects "
      "activate across the wide view and the players' area widens (R4's second mode, R20); 0 = 4:3", "0" },
    { "high_fps", { "0", "120", "144", "165", "240", nullptr }, "0",
      "the display rate in Hz with frames interpolated toward each logic frame (R3), 0 = off; any 61-1000", "0" },
    { "unlimited_sprites", { "0", "1", nullptr }, "0",
      "no per-line sprite limits and no 80-entry cap on the game's sprite list (R10)", "0" },
    { "music_voices", { "1", "0", nullptr }, "0",
      "the music on sound chips of its own, so no sound effect takes its notes (R13, R14); 0 = one set of "
      "chips, as the cartridge", "1" },
    { "fm_ladder", { "1", "0", nullptr }, "1",
      "the YM2612's ladder effect (the cartridge's); 0 = the ladder-free chip (R15)", "1" },
    { "fm_filter", { "0", "1", nullptr }, "0",
      "1 = a cleaner output low-pass than the cartridge's (R15)", "0" },
    { "weapon_level", { "0", "1", "2", "3", "4", "5", "6", nullptr }, "0",
      "the weapon level every player starts with, 1-6 (changes the game, R24); 0 = the cartridge's", "0" },
    /* the level select (the X-Men 2 harvest, 2026-11-20): k_start_sections' names, in the game's order */
    { "start_stage", { "0", "1-street", "1-gem-expo", "1-warehouse", "1-parade", "2", "2-lift", "2-flight",
                       "2-clouds", "2-airship", "2-beam-ship", "3", "3-backlot", "3-cat-head", "3-mushrooms",
                       "3-spotted-room", "3-card-hall", "3-hatter", "4", "4-hall", "4-lab", "4-tube", "4-freeze",
                       nullptr }, "0",
      "level select (changes the game): every new game starts at this section, after the player select as "
      "usual, and carries on from there in the game's own order; a number = that stage's opening (the "
      "Batmobile and its title card), N-name = a section of stage N; 0 = stage 1's opening, the cartridge's",
      "0" },
    /* R32 (the user, 2026-11-25: xmen2's three ways and its default; the kit's porting.md "Scaling modes"):
       presentation only, the window's alone (no trace, digest or present log sees it). R35 (the user,
       2026-11-26): on by default, like music_voices; the cartridge setting is integer, every game pixel
       the same whole size */
    { "scaling", { "sharp_bilinear", "integer", "nearest", nullptr }, "integer",
      "how the picture is enlarged to the window, in played runs (F4 switches it): sharp_bilinear = filling the "
      "window with every game pixel the same size, a one-screen-pixel blend where two meet; integer = the largest "
      "whole multiple that fits, sharp, with a border; nearest = filling the window, sharp, with some pixels a "
      "screen pixel wider or taller than others at most sizes (the only way before R32). At a whole multiple, "
      "such as the 3x window, all three are the same (R32)", "sharp_bilinear" },
    /* R34 (the user, 2026-11-25; porting.md "Shader presets"): any path, "" = none (the plain picture) */
    { "shader_preset", { "", nullptr }, "",
      "a RetroArch shader preset (.slangp) the picture goes through, in played runs (librashader beside the "
      "game; F8 turns it off and on): a path, relative ones from the game's folder; empty = none, the plain "
      "picture (R34)", "" },
};
static const int NDEFS = (int)(sizeof s_defs / sizeof s_defs[0]);

/* start_stage's sections: the section id each value starts at, and the one before it in the game's
   section order (Tbl_009FFC, (from, to) word pairs: 0 -> $0001 -> $1100 -> $1300 -> ... -> $4401).
   Main_Start's new game sets g_SectionId to the one before (game/c/009E02.c, OPT_START_SECTION), so
   Section_RunNext's first step goes to the chosen one, as it goes there after the one before ends
   (the section-skip code ends a section the same way). Every one is a routine that sets up its whole
   scene (Tbl_00A0AC; each but $4401 is also where the continue table Tbl_00A05C restarts a game). */
static const struct { const char *name; unsigned section, before; } k_start_sections[] = {
    { "1-street", 0x1100, 0x0001 }, { "1-gem-expo", 0x1300, 0x1100 }, { "1-warehouse", 0x1301, 0x1300 },
    { "1-parade", 0x1400, 0x1301 }, { "2", 0x0002, 0x1400 }, { "2-lift", 0x2100, 0x0002 },
    { "2-flight", 0x2200, 0x2100 }, { "2-clouds", 0x2300, 0x2200 }, { "2-airship", 0x2400, 0x2300 },
    { "2-beam-ship", 0x2401, 0x2400 }, { "3", 0x0003, 0x2401 }, { "3-backlot", 0x3100, 0x0003 },
    { "3-cat-head", 0x3101, 0x3100 }, { "3-mushrooms", 0x3200, 0x3101 }, { "3-spotted-room", 0x3300, 0x3200 },
    { "3-card-hall", 0x3400, 0x3300 }, { "3-hatter", 0x3401, 0x3400 }, { "4", 0x0004, 0x3401 },
    { "4-hall", 0x4000, 0x0004 }, { "4-lab", 0x4300, 0x4000 }, { "4-tube", 0x4400, 0x4300 },
    { "4-freeze", 0x4401, 0x4400 },
};

static std::string s_ini_path, s_exe_dir;
static int s_ini_found;
static std::string s_ini_rom;   /* [rom] path */
/* every section's keys as read (and as written since) */
static std::map<std::string, std::map<std::string, std::string>> s_ini;

static OptDef *find(const std::string &k)
{
    for (OptDef &d : s_defs)
        if (k == d.name) return &d;
    return nullptr;
}

/* high_fps takes any rate from 61 to 1000 Hz besides the listed ones */
static bool valid(const OptDef &d, const std::string &v)
{
    for (int i = 0; d.values[i]; i++)
        if (v == d.values[i]) return true;
    if (!strcmp(d.name, "shader_preset"))   /* a path: one line, and no comma (--options K=V,K=V) */
        return v.find_first_of(",\r\n") == std::string::npos;
    char *e;
    long n = strtol(v.c_str(), &e, 10);
    if (v.empty() || *e) return false;
    if (!strcmp(d.name, "high_fps")) return n >= 61 && n <= 1000;
    return false;
}

static std::string trim(const std::string &s)
{
    size_t a = s.find_first_not_of(" \t\r\n"), b = s.find_last_not_of(" \t\r\n");
    return a == std::string::npos ? std::string() : s.substr(a, b - a + 1);
}

static void set(const std::string &k, const std::string &v, const char *from)
{
    OptDef *d = find(k);
    if (!d) { fprintf(stderr, "[native] %s: unknown option %s\n", from, k.c_str()); exit(2); }
    if (!valid(*d, v)) { fprintf(stderr, "[native] %s: %s=%s is not one of its values\n", from, k.c_str(), v.c_str()); exit(2); }
    d->value = v;
}

static std::string exe_dir(const char *argv0)
{
    char buf[4096] = "";
#ifdef _WIN32
    DWORD n = GetModuleFileNameA(nullptr, buf, (DWORD)sizeof buf);
    if (n == 0 || n >= sizeof buf) snprintf(buf, sizeof buf, "%s", argv0);
#else
    ssize_t n = readlink("/proc/self/exe", buf, sizeof buf - 1);
    if (n > 0) buf[n] = 0; else snprintf(buf, sizeof buf, "%s", argv0);
#endif
    std::string p = buf;
    size_t s = p.find_last_of("/\\");
    return s == std::string::npos ? std::string(".") : p.substr(0, s);
}

/* settings.ini beside the runner (the kit writes its [input.pN] sections there too): [options] sets
   the options; every section is kept for the front door ([rom] path, [launcher], [playerN]) */
static void read_ini(void)
{
    FILE *f = fopen(s_ini_path.c_str(), "r");
    if (!f) return;
    s_ini_found = 1;
    char line[4096];
    std::string section;
    while (fgets(line, sizeof line, f)) {
        std::string s = trim(line);
        if (s.empty() || s[0] == '#' || s[0] == ';') continue;
        if (s[0] == '[') { section = s; continue; }
        size_t eq = s.find('=');
        if (eq == std::string::npos) continue;
        std::string k = trim(s.substr(0, eq)), v = trim(s.substr(eq + 1));
        if (section == "[options]") set(k, v, s_ini_path.c_str());
        else if (section == "[rom]" && k == "path") s_ini_rom = v;
        if (section.size() > 2) s_ini[section.substr(1, section.size() - 2)][k] = v;
    }
    fclose(f);
}

static void apply(void)
{
    const std::string &ws = find("widescreen")->value;
    g_opt.widescreen = ws != "0";
    g_opt.widescreen_gameplay = ws == "16:9-gameplay";
    g_opt.high_fps = atoi(find("high_fps")->value.c_str());
    g_opt.unlimited_sprites = find("unlimited_sprites")->value == "1";
    g_opt.music_voices = find("music_voices")->value == "1";
    g_opt.fm_ladder = find("fm_ladder")->value == "1";
    g_opt.fm_filter = find("fm_filter")->value == "1";
    g_opt.weapon_level = atoi(find("weapon_level")->value.c_str());
    g_opt.start_section = 0;
    const std::string &st = find("start_stage")->value;
    for (const auto &s : k_start_sections)
        if (st == s.name) g_opt.start_section = (int)s.before;
}

/* the game's native branches read them (recomp/game/include/dk_opts.h, Main_Start) */
extern "C" int opt_weapon_level(void) { return g_opt.weapon_level; }
extern "C" int opt_start_section(void) { return g_opt.start_section; }

static std::string json_str(const std::string &s)
{
    std::string o = "\"";
    for (char c : s) {
        if (c == '"' || c == '\\') o += '\\';
        if ((unsigned char)c < 0x20) { char b[8]; snprintf(b, sizeof b, "\\u%04x", c); o += b; continue; }
        o += c;
    }
    return o + "\"";
}

std::string options_effective_json(void)
{
    std::string o = "{";
    for (int i = 0; i < NDEFS; i++)
        o += (i ? ", " : "") + json_str(s_defs[i].name) + ": " + json_str(s_defs[i].value);
    return o + "}";
}

static void print_json(void)
{
    std::string o = "{\"options\": {";
    for (int i = 0; i < NDEFS; i++) {
        const OptDef &d = s_defs[i];
        o += (i ? ", " : "") + json_str(d.name) + ": {\"default\": " + json_str(d.values[0]) + ", \"values\": [";
        for (int k = 0; d.values[k]; k++) o += (k ? ", " : "") + json_str(d.values[k]);
        o += "], \"cartridge\": " + json_str(d.cartridge) + ", \"value\": " + json_str(d.value) +
             ", \"what\": " + json_str(d.what) + "}";
    }
    o += "}, \"effective\": " + options_effective_json() + ", \"settings_file\": " + json_str(s_ini_path) +
         ", \"settings_file_found\": " + (s_ini_found ? "true" : "false") + "}";
    printf("%s\n", o.c_str());
    fflush(stdout);
}

/* Played runs (host_sdl.cpp, R1's hotkeys): F9 turns high_fps off and back to its rate (144 if none
   was given); F10 steps widescreen 0 -> 16:9 -> 16:9-gameplay -> 0 (the user, 2026-11-17: "widescreen
   should toggle between off, on, gameplay"; the widescreen code reads g_opt each frame and takes the
   change at its next safe point). Each shows its new setting on the picture (host_osd, as X-Men 2's). */
void options_toggle_high_fps(void)
{
    static int rate = 144;
    if (g_opt.high_fps) { rate = g_opt.high_fps; g_opt.high_fps = 0; }
    else g_opt.high_fps = rate;
    find("high_fps")->value = std::to_string(g_opt.high_fps);
    crash_log_option("high_fps", find("high_fps")->value.c_str());
    fprintf(stderr, "[native] F9: high_fps=%d\n", g_opt.high_fps);
    host_osd(g_opt.high_fps ? ("HIGH FPS: " + std::to_string(g_opt.high_fps)).c_str() : "HIGH FPS: OFF");
}

void options_toggle_widescreen(void)
{
    std::string &v = find("widescreen")->value;
    v = v == "0" ? "16:9" : v == "16:9" ? "16:9-gameplay" : "0";
    apply();
    crash_log_option("widescreen", v.c_str());
    fprintf(stderr, "[native] F10: widescreen=%s\n", v.c_str());
    host_osd(v == "0" ? "WIDESCREEN: OFF" : v == "16:9" ? "WIDESCREEN: 16:9" : "WIDESCREEN: 16:9 GAMEPLAY");
}

/* scaling (R32; host_sdl.cpp draw): F4 goes through the three in the table's order, each said on the
   picture (the window's alone: nothing logged for a replay, which draws no window) */
int options_scaling(void)
{
    const std::string &v = find("scaling")->value;
    return v == "integer" ? SCALE_INTEGER : v == "nearest" ? SCALE_NEAREST : SCALE_SHARP_BILINEAR;
}

void options_toggle_scaling(void)
{
    static const char *const said[] = { "SCALING: EVEN PIXELS", "SCALING: WHOLE NUMBER", "SCALING: SHARP" };
    int m = (options_scaling() + 1) % 3;
    OptDef *d = find("scaling");
    d->value = d->values[m];   /* the table's order is the modes' */
    fprintf(stderr, "[native] F4: scaling=%s\n", d->value.c_str());
    host_osd(said[m]);
}

std::string options_shader_preset(void) { return find("shader_preset")->value; }

/* An input script's OPTION line (input_script.cpp): a played session's F9/F10 change, replayed
   where it happened (crash_report.h). 0 for an unknown option or value. */
int options_set_now(const char *name, const char *value)
{
    OptDef *d = find(name);
    if (!d || !valid(*d, value)) return 0;
    d->value = value;
    apply();
    return 1;
}

/* ---- the front door: the settings file's other sections, the table for the launcher ---- */

std::string options_exe_dir(void) { return s_exe_dir; }
std::string options_ini_path(void) { return s_ini_path; }
std::string options_ini_rom(void) { return s_ini_rom; }

std::string options_ini_get(const char *section, const char *key, const std::string &def)
{
    auto s = s_ini.find(section);
    if (s == s_ini.end()) return def;
    auto k = s->second.find(key);
    return k == s->second.end() ? def : k->second;
}

bool options_ini_write(const char *section, const std::vector<std::pair<std::string, std::string>> &kv,
                       const char *comment)
{
    std::string head = std::string("[") + section + "]";
    std::vector<std::string> keep;
    if (FILE *f = fopen(s_ini_path.c_str(), "r")) {
        char line[4096];
        bool in = false;
        while (fgets(line, sizeof line, f)) {
            std::string s = trim(line);
            if (!s.empty() && s[0] == '[') in = s == head;
            if (in) continue;
            std::string l = line;
            while (!l.empty() && (l.back() == '\n' || l.back() == '\r')) l.pop_back();
            keep.push_back(l);
        }
        fclose(f);
    }
    while (!keep.empty() && trim(keep.back()).empty()) keep.pop_back();
    FILE *f = fopen(s_ini_path.c_str(), "w");
    if (!f) return false;
    for (const std::string &l : keep) fprintf(f, "%s\n", l.c_str());
    if (!kv.empty()) {
        fprintf(f, "%s%s\n", keep.empty() ? "" : "\n", head.c_str());
        if (comment && *comment) fprintf(f, "# %s\n", comment);
        for (const auto &p : kv) fprintf(f, "%s = %s\n", p.first.c_str(), p.second.c_str());
    }
    bool ok = fclose(f) == 0;
    if (ok) {
        s_ini_found = 1;
        auto &m = s_ini[section];
        m.clear();
        for (const auto &p : kv) m[p.first] = p.second;
    }
    return ok;
}

bool options_remember_rom(const std::string &path)
{
    bool ok = options_ini_write("rom", {{"path", path}}, "the ROM the game plays when started without one (set when you chose it)");
    if (ok) s_ini_rom = path;
    return ok;
}

int options_count(void) { return NDEFS; }
const char *options_name(int i) { return s_defs[i].name; }
const char *options_what(int i) { return s_defs[i].what; }
const char *options_cartridge(int i) { return s_defs[i].cartridge; }
std::vector<std::string> options_values(int i)
{
    std::vector<std::string> v;
    for (int k = 0; s_defs[i].values[k]; k++) v.push_back(s_defs[i].values[k]);
    return v;
}
std::string options_get(int i) { return s_defs[i].value; }
bool options_set(int i, const std::string &v)
{
    if (!valid(s_defs[i], v)) return false;
    s_defs[i].value = v;
    apply();
    return true;
}

bool options_save(void)
{
    std::vector<std::pair<std::string, std::string>> kv;
    for (const OptDef &d : s_defs)
        if (d.value != d.values[0]) kv.emplace_back(d.name, d.value);
    return options_ini_write("options", kv, "changed from the defaults (the launcher writes this section; "
                                            "batmanrobin_native --options --json lists every option)");
}

/* Removes --options and its argument from argv (main parses the rest); exits after --options --json. */
void options_init(int *argc, char **argv)
{
    s_exe_dir = exe_dir(argv[0]);
    s_ini_path = s_exe_dir + "/settings.ini";
    read_ini();
    bool list = false;
    int w = 1;
    for (int i = 1; i < *argc; i++) {
        if (!strcmp(argv[i], "--options") && i + 1 < *argc) {
            const char *a = argv[++i];
            if (!strcmp(a, "--json")) { list = true; continue; }
            std::string s = a;
            for (size_t p = 0; p <= s.size(); ) {
                size_t c = s.find(',', p);
                if (c == std::string::npos) c = s.size();
                std::string kv = trim(s.substr(p, c - p));
                p = c + 1;
                if (kv.empty()) continue;
                size_t eq = kv.find('=');
                if (eq == std::string::npos) { fprintf(stderr, "[native] --options: %s is not K=V\n", kv.c_str()); exit(2); }
                set(trim(kv.substr(0, eq)), trim(kv.substr(eq + 1)), "--options");
            }
            continue;
        }
        argv[w++] = argv[i];
    }
    *argc = w;
    argv[w] = nullptr;
    apply();
    if (list) { print_json(); exit(0); }
}
