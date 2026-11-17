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
 * Adapted from the X-Men 2 port's platform/native/options.cpp (the same kit).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "options.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif

NativeOptions g_opt = {
    /* widescreen */ 0, /* widescreen_gameplay */ 0, /* high_fps */ 0, /* unlimited_sprites */ 0,
    /* music_voices */ 1, /* fm_ladder */ 1, /* fm_filter */ 0,
};

struct OptDef {
    const char *name;
    const char *values[8];    /* the first is the default; null-terminated */
    const char *cartridge;    /* the original's setting: what every check against phase 5 runs with */
    const char *what;
    std::string value;
};

static OptDef s_defs[] = {
    { "widescreen", { "0", "16:9", nullptr }, "0",
      "the 16:9 view (R4, R5): presentation only unless widescreen_gameplay; 0 = 4:3", "0" },
    { "widescreen_gameplay", { "0", "1", nullptr }, "0",
      "with widescreen: objects activate across the wide view and the camera keeps it inside the level "
      "(R4's second mode); 0 = the game plays as in 4:3", "0" },
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
};
static const int NDEFS = (int)(sizeof s_defs / sizeof s_defs[0]);
static std::string s_ini_path;
static int s_ini_found;

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

/* settings.ini beside the runner (the kit writes its [input.pN] sections there too): [options] */
static void read_ini(void)
{
    FILE *f = fopen(s_ini_path.c_str(), "r");
    if (!f) return;
    s_ini_found = 1;
    char line[512];
    bool in_options = false;
    while (fgets(line, sizeof line, f)) {
        std::string s = trim(line);
        if (s.empty() || s[0] == '#' || s[0] == ';') continue;
        if (s[0] == '[') { in_options = s == "[options]"; continue; }
        size_t eq = s.find('=');
        if (!in_options || eq == std::string::npos) continue;
        set(trim(s.substr(0, eq)), trim(s.substr(eq + 1)), s_ini_path.c_str());
    }
    fclose(f);
}

static void apply(void)
{
    g_opt.widescreen = find("widescreen")->value == "16:9";
    g_opt.widescreen_gameplay = find("widescreen_gameplay")->value == "1";
    g_opt.high_fps = atoi(find("high_fps")->value.c_str());
    g_opt.unlimited_sprites = find("unlimited_sprites")->value == "1";
    g_opt.music_voices = find("music_voices")->value == "1";
    g_opt.fm_ladder = find("fm_ladder")->value == "1";
    g_opt.fm_filter = find("fm_filter")->value == "1";
}

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

/* Played runs (host_sdl.cpp, R1's hotkeys for the presentation options): F9 turns high_fps off and
   back to its rate (144 if none was given); F10 turns the 16:9 view on and off (the widescreen code
   reads g_opt.widescreen each frame and takes the change at its next safe point). */
void options_toggle_high_fps(void)
{
    static int rate = 144;
    if (g_opt.high_fps) { rate = g_opt.high_fps; g_opt.high_fps = 0; }
    else g_opt.high_fps = rate;
    find("high_fps")->value = std::to_string(g_opt.high_fps);
    fprintf(stderr, "[native] F9: high_fps=%d\n", g_opt.high_fps);
}

void options_toggle_widescreen(void)
{
    g_opt.widescreen = !g_opt.widescreen;
    find("widescreen")->value = g_opt.widescreen ? "16:9" : "0";
    fprintf(stderr, "[native] F10: widescreen=%s\n", g_opt.widescreen ? "16:9" : "0");
}

/* Removes --options and its argument from argv (main parses the rest); exits after --options --json. */
void options_init(int *argc, char **argv)
{
    s_ini_path = exe_dir(argv[0]) + "/settings.ini";
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
