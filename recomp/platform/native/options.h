/* options.h - the native runner's phase-6 options (docs/phases/P6-enhance.md, R1).
 *
 * One record every part of the native runner reads; the options layer (options.cpp: the command
 * line `--options K=V,...`, the settings file beside the exe, `--options --json`) fills it before
 * the machine starts. Each option is off by default except music_voices (R13); every check that
 * compares with the original runs with every field at its cartridge setting (0, fm_ladder 1).
 */
#pragma once

struct NativeOptions {
    int widescreen;           /* 0 = 4:3, 1 = 16:9 (R4, R5) */
    int widescreen_gameplay;  /* 1 = objects activate across the wide view (R4's second mode) */
    int high_fps;             /* 0 = off, else the present rate in Hz (R3) */
    int unlimited_sprites;    /* 1 = no per-line limits, no 80-entry cap (R10) */
    int music_voices;         /* 1 = the music on a sound machine of its own (R13, R14) */
    int fm_ladder;            /* 1 = the YM2612's ladder effect (the cartridge), 0 = removed (R15) */
    int fm_filter;            /* 0 = the cartridge's output low-pass, 1 = the cleaner one (R15) */
};

extern NativeOptions g_opt;

#ifdef __cplusplus
#include <string>
/* The settings file beside the exe ([options]), then `--options K=V,...` (removed from argv);
   `--options --json` prints every option and exits 0. Call first in main, before the ROM. */
void options_init(int *argc, char **argv);
std::string options_effective_json(void);   /* {"name": "value", ...}: the trace's "options" */
void options_toggle_high_fps(void);         /* played runs: F9 */
void options_toggle_widescreen(void);       /* played runs: F10 */
#endif
