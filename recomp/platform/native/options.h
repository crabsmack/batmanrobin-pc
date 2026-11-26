/* options.h - the native runner's phase-6 options (docs/phases/P6-enhance.md, R1).
 *
 * One record every part of the native runner reads; the options layer (options.cpp: the command
 * line `--options K=V,...`, the settings file beside the exe, `--options --json`) fills it before
 * the machine starts. Each option is off by default except music_voices (R13); every check that
 * compares with the original runs with every field at its cartridge setting (0, fm_ladder 1).
 */
#pragma once

struct NativeOptions {
    int widescreen;           /* 0 = 4:3, 1 = 16:9 (R4, R5): the option widescreen = 16:9 or 16:9-gameplay */
    int widescreen_gameplay;  /* 1 = objects activate across the wide view (R4's second mode, R20):
                                 widescreen = 16:9-gameplay (R23: one setting) */
    int high_fps;             /* 0 = off, else the present rate in Hz (R3) */
    int unlimited_sprites;    /* 1 = no per-line limits, no 80-entry cap (R10) */
    int music_voices;         /* 1 = the music on a sound machine of its own (R13, R14) */
    int fm_ladder;            /* 1 = the YM2612's ladder effect (the cartridge), 0 = removed (R15) */
    int fm_filter;            /* 0 = the cartridge's output low-pass, 1 = the cleaner one (R15) */
    int weapon_level;         /* 0 = the cartridge's start weapon level, 1-6 = start at it (R24) */
    int start_section;        /* start_stage: 0 = the cartridge's (g_SectionId 0 at a new game), else the
                                 section id the order table steps from to reach the chosen start */
};

extern NativeOptions g_opt;

#ifdef __cplusplus
#include <string>
#include <utility>
#include <vector>
/* The settings file beside the exe ([options]), then `--options K=V,...` (removed from argv);
   `--options --json` prints every option and exits 0. Call first in main, before the ROM. */
void options_init(int *argc, char **argv);
std::string options_effective_json(void);   /* {"name": "value", ...}: the trace's "options" */
void options_toggle_high_fps(void);         /* played runs: F9 */
void options_toggle_widescreen(void);       /* played runs: F10 */
int options_set_now(const char *name, const char *value);   /* an input script's OPTION line */
/* The window's own options (R32, R34): presentation only, read by host_sdl.cpp and shader.cpp, never
   by the game (no NativeOptions field, no trace sees them) */
enum { SCALE_SHARP_BILINEAR, SCALE_INTEGER, SCALE_NEAREST };   /* scaling's values, in the table's order */
int options_scaling(void);                  /* SCALE_* */
void options_toggle_scaling(void);          /* played runs: F4, the next of the three */
std::string options_shader_preset(void);    /* shader_preset as given ("" = none) */

/* The front door (runner.cpp, launcher.cpp, controls.cpp): the folder the runner is in, the settings
   file's other sections ([rom], [launcher], [player1], [player2]; the kit's [input.pN] are kept as
   they are), and the options table for the launcher. */
std::string options_exe_dir(void);
std::string options_ini_path(void);
std::string options_ini_get(const char *section, const char *key, const std::string &def);
/* settings.ini's [section] becomes kv (after a `# comment` line); no kv: the section goes. The rest of
   the file stays as it was (a new file if none). false: it could not be written. */
bool options_ini_write(const char *section, const std::vector<std::pair<std::string, std::string>> &kv,
                       const char *comment);
std::string options_ini_rom(void);                     /* [rom] path: the ROM last chosen ("" none) */
bool options_remember_rom(const std::string &path);    /* writes [rom] path */
int options_count(void);
const char *options_name(int i);
const char *options_what(int i);
const char *options_cartridge(int i);                  /* the cartridge's setting of option i */
std::vector<std::string> options_values(int i);        /* the default first */
std::string options_get(int i);
bool options_set(int i, const std::string &v);         /* false: not one of its values */
bool options_save(void);                               /* [options]: every option not at its default */
#endif
