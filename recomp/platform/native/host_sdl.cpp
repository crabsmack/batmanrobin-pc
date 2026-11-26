/* host_sdl.cpp - the native runner's window, input and sound when played (phase 5 P6/P8). SDL2 for
 * the window, the keyboard, game controllers and the audio device; the sound chips and their mixer
 * are the engine's (audio/: ymfm's YM2612, the SN76489, the cycle-stamped event queue the Z80's and
 * the PSG's writes go into, audio.c's device bridge).
 *
 *   The players' keys and controllers are controls.cpp's (the launcher's Controls page, settings.ini's
 *   [player1] and [player2]); by default the keyboard is player 1's: arrows; Z = A, X = B, C = C,
 *   Enter = Start; game controllers are players 1-2 in plug-in order (the keyboard with the first; an
 *   unplugged one frees its player): D-pad or left stick; X = A, A = B, B = C, Start (face buttons
 *   by position). F11 toggles fullscreen; Esc or closing the window quits.
 *   F9 turns high_fps off and on, F10 steps widescreen through 0, 16:9 and 16:9-gameplay (phase 6's
 *   options, options.cpp); F4 the next scaling (R32: sharp_bilinear, integer, nearest: draw()), F8 the
 *   shader preset off and on (R34: shader_preset, shader.cpp); each hotkey shows its new setting on the
 *   picture for 2 s (host_osd).
 * Frames are paced to the NTSC rate (59.92 Hz) by the clock. At exit a played run prints
 * `[native] picture {...}` (the renderer, the window, scaling and its F4 presses, the shader's state).
 * Testing (played runs, R32/R34's checks: tools/present_check.py): BR_WINDOW_SIZE=W,H the window's size
 * in place of 3x; BR_BACKBUFFER_SHOT=N,M,... with BR_DUMP_DIR: the window as presented at the Nth picture
 * drawn (after the chain and the message) as backbuffer_N.bmp, the game's picture it was drawn from as
 * frame_N.bmp, and the run quits after the last; BR_HOTKEY_TEST also takes F4 and F8.
 * The front door (runner.cpp): message boxes (host_message; BR_NO_MESSAGEBOX=1 or --no-messagebox
 * print them instead, as does any headless run), the ROM picker, the console a double-click opened.
 * BR_PAD_SELFTEST=1: the controllers' slots checked with SDL's virtual controllers, headless.
 *
 * Adapted from the X-Men 2 port's platform/native/host_sdl.cpp (the same kit).
 */
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <algorithm>

extern "C" {
#include "audio.h"
#include "audio/mixer.h"
#include "genesis_clocks.h"
#include "video/genesis_bus.h"
}
#include "controls.h"
#include "host_sdl.h"
#include "options.h"
#include "osd_font.h"
#include "shader.h"
#include "sound2.h"
#include "rewind.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <commdlg.h>
#else
#include <sys/wait.h>
#endif

#define BR_TITLE "The Adventures of Batman and Robin"

/* ---- the front door ---------------------------------------------------------------------------- */

/* Windows: the runner is a console program (its output is for the command line and the harness), so a
   start from Explorer gets a console window of its own, empty but for log lines. A played run that is
   the only process on its console (a double-click; a terminal or a script shares theirs) lets it go:
   the window closes, and what a player must know comes in message boxes. Output a parent redirected
   (pipes, files) is unaffected; a process started with no console window (CREATE_NO_WINDOW, the
   harness's background runs) has none to close. */
void host_detach_lone_console(void)
{
#ifdef _WIN32
    DWORD pids[2];
    if (GetConsoleWindow() && GetConsoleProcessList(pids, 2) == 1) FreeConsole();
#endif
}

static int s_print_messages;

void host_messages_print(int on) { s_print_messages = on; }

/* No dialog may wait on the desktop for a person who isn't there (SDL shows real message boxes on
   Windows even with the dummy video driver): --no-messagebox, BR_NO_MESSAGEBOX=1, the kit's harness
   environment (GENESIS_NO_LAUNCHER=1) and a headless video driver all print instead. */
int host_messages_printed(void)
{
    if (s_print_messages) return 1;
    const char *e = getenv("BR_NO_MESSAGEBOX");
    if (e && *e && strcmp(e, "0")) return 1;
    e = getenv("GENESIS_NO_LAUNCHER");
    if (e && *e && strcmp(e, "0")) return 1;
    e = getenv("SDL_VIDEODRIVER");
    if (e && (!strcmp(e, "dummy") || !strcmp(e, "offscreen"))) return 1;
    return 0;
}

void host_message(const char *text)
{
    if (host_messages_printed()) {
        fprintf(stderr, "[native] message: %s\n", text);
        fflush(stderr);
        return;
    }
    if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, BR_TITLE, text, nullptr) != 0)
        fprintf(stderr, "[native] %s\n", text);
}

void host_message_error(const char *text)
{
    if (host_messages_printed()) {
        fprintf(stderr, "[native] message: %s\n", text);
        fflush(stderr);
        return;
    }
    if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, BR_TITLE, text, nullptr) != 0)
        fprintf(stderr, "[native] %s\n", text);
}

/* a file picker: filter is Windows' ("Name\0*.a;*.b\0...\0"), glob the same patterns for zenity and
   kdialog ("*.a *.b"); titles have no single quote */
static bool pick_file(char *path, size_t cap, const char *filter, const char *glob, const char *title,
                      const char *title_unix)
{
#ifdef _WIN32
    (void)glob;
    (void)title_unix;
    OPENFILENAMEA o = {};
    path[0] = 0;
    o.lStructSize = sizeof o;
    o.lpstrFilter = filter;
    o.lpstrFile = path;
    o.nMaxFile = (DWORD)cap;
    o.lpstrTitle = title;
    o.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    return GetOpenFileNameA(&o) != 0;
#else
    (void)filter;
    (void)title;
    /* zenity (GNOME and most desktops) or kdialog (KDE); neither: false */
    std::string z = std::string("zenity --file-selection --title='") + title_unix + "' --file-filter='" + glob +
                    "' --file-filter='*' 2>/dev/null";
    std::string k = std::string("kdialog --getopenfilename . '") + glob + "' 2>/dev/null";
    const char *const cmds[] = { z.c_str(), k.c_str() };
    for (const char *cmd : cmds) {
        FILE *p = popen(cmd, "r");
        if (!p) continue;
        bool got = fgets(path, (int)cap, p) != nullptr;
        int rc = pclose(p);
        if (got) {
            size_t n = strlen(path);
            while (n && (path[n - 1] == '\n' || path[n - 1] == '\r')) path[--n] = 0;
            if (n) return true;
        }
        if (WIFEXITED(rc) && WEXITSTATUS(rc) == 1) return false;   /* it ran and was cancelled: not the next one */
    }
    return false;
#endif
}

bool host_pick_rom(char *path, size_t cap)
{
    if (const char *t = getenv("BR_PICK_ROM_TEST")) {   /* testing: the picker's answer ("" = cancelled) */
        snprintf(path, cap, "%s", t);
        return *t != 0;
    }
    if (host_messages_printed()) return false;   /* nobody to choose */
    return pick_file(path, cap, "Genesis / Mega Drive ROM (*.md, *.bin, *.gen, *.smd)\0*.md;*.bin;*.gen;*.smd\0All files\0*.*\0",
                     "*.md *.bin *.gen *.smd", "The Adventures of Batman and Robin - choose your ROM of the cartridge (USA)",
                     "The Adventures of Batman and Robin - choose your ROM (USA)");
}

/* the launcher's Browse... for shader_preset (R34) */
bool host_pick_shader(char *path, size_t cap)
{
    if (const char *t = getenv("BR_PICK_SHADER_TEST")) {   /* testing: the picker's answer ("" = cancelled) */
        snprintf(path, cap, "%s", t);
        return *t != 0;
    }
    if (host_messages_printed()) return false;
    return pick_file(path, cap, "RetroArch shader preset (*.slangp)\0*.slangp\0All files\0*.*\0", "*.slangp",
                     "The Adventures of Batman and Robin - choose a shader preset (.slangp)",
                     "The Adventures of Batman and Robin - choose a shader preset (.slangp)");
}

/* ---- the window --------------------------------------------------------------------------------- */

static SDL_Window *s_win;
static SDL_Renderer *s_ren;
static SDL_Texture *s_tex;
static SDL_Texture *s_osd_tex;   /* shader_preset: the hotkeys' messages, drawn over the shaded picture */
static SDL_Texture *s_pre;       /* scaling=sharp_bilinear: the picture at a whole multiple, smoothed from there */
static int s_pre_w, s_pre_h;
static uint64_t s_next;
static double s_period;
static int s_audio;
static uint64_t s_scaling_keys;  /* F4 presses (the exit's picture line) */

static int make_renderer(void)
{
    shader_close();   /* its objects are the old device's */
    if (s_osd_tex) { SDL_DestroyTexture(s_osd_tex); s_osd_tex = nullptr; }
    if (s_pre) { SDL_DestroyTexture(s_pre); s_pre = nullptr; s_pre_w = s_pre_h = 0; }
    if (s_tex) { SDL_DestroyTexture(s_tex); s_tex = nullptr; }
    if (s_ren) { SDL_DestroyRenderer(s_ren); s_ren = nullptr; }
#ifdef _WIN32
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d11");
    s_ren = SDL_CreateRenderer(s_win, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "");
#else
    if (!options_shader_preset().empty()) {   /* shader.cpp's runtime there is OpenGL; else SDL's own choice */
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
        s_ren = SDL_CreateRenderer(s_win, -1, SDL_RENDERER_ACCELERATED);
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "");
    }
#endif
    if (!s_ren) s_ren = SDL_CreateRenderer(s_win, -1, SDL_RENDERER_ACCELERATED);
    if (!s_ren) s_ren = SDL_CreateRenderer(s_win, -1, 0);
    if (!s_ren) { fprintf(stderr, "[native] SDL renderer: %s\n", SDL_GetError()); return 0; }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    s_tex = SDL_CreateTexture(s_ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 512, 480);
    if (!s_tex) { fprintf(stderr, "[native] SDL texture: %s\n", SDL_GetError()); return 0; }
    SDL_SetTextureScaleMode(s_tex, SDL_ScaleModeNearest);
    shader_open(s_ren);   /* shader_preset (shader.cpp); nothing when it is unset */
    if (!options_shader_preset().empty()) {
        s_osd_tex = SDL_CreateTexture(s_ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 512, 480);
        if (s_osd_tex) SDL_SetTextureBlendMode(s_osd_tex, SDL_BLENDMODE_BLEND);
    }
    return 1;
}

/* the exit's `[native] picture {...}` line (a player's log says how the window was drawn: RELEASING.md
   "The log is a triage record") */
static void picture_line(void)
{
    SDL_RendererInfo info = {};
    int ww = 0, wh = 0;
    if (s_ren) { SDL_GetRendererInfo(s_ren, &info); SDL_GetRendererOutputSize(s_ren, &ww, &wh); }
    int fs = s_win && (SDL_GetWindowFlags(s_win) & SDL_WINDOW_FULLSCREEN_DESKTOP) ? 1 : 0;
    static const char *const modes[] = { "sharp_bilinear", "integer", "nearest" };
    fprintf(stderr, "[native] picture {\"renderer\": \"%s\", \"window\": [%d, %d], \"fullscreen\": %d, \"scaling\": \"%s\", "
                    "\"scaling_keys\": %llu, \"shader\": \"%s\", \"shader_frames\": %llu}\n",
            info.name ? info.name : "?", ww, wh, fs, modes[options_scaling()], (unsigned long long)s_scaling_keys,
            shader_state(), (unsigned long long)shader_frames());
}

/* The controllers (controls.cpp): each player's, plugged in and out while playing. A change shows on
   the picture ("PLAYER 2: XBOX CONTROLLER") once the game is running; host_open's own scan doesn't. */
static bool s_pads_osd;

static void pad_event(const SDL_Event &e)
{
    std::string msg = controls_event(e);
    if (!msg.empty() && s_pads_osd) {
        for (char &c : msg) c = (char)toupper((unsigned char)c);
        if (msg.size() > 46) msg.resize(46);
        host_osd(msg.c_str());
    }
}

int host_open(const char *title)
{
    SDL_SetMainReady();
    controls_sdl_hints();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS) != 0) {
        fprintf(stderr, "[native] SDL: %s\n", SDL_GetError());
        return 0;
    }
    s_win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320 * 3, 224 * 3,
                             SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!s_win) { fprintf(stderr, "[native] SDL window: %s\n", SDL_GetError()); return 0; }
    int tw, th;   /* testing: BR_WINDOW_SIZE=W,H the window's size in place of 3x (R32's check) */
    if (const char *t = getenv("BR_WINDOW_SIZE"))
        if (sscanf(t, "%d,%d", &tw, &th) == 2 && tw > 0 && th > 0) SDL_SetWindowSize(s_win, tw, th);
    if (!make_renderer()) return 0;
    controls_devices_init();   /* settings.ini's [playerN], then every controller plugged in */
    audio_mixer_init();
    s_audio = audio_init((int)GENESIS_PSG_SAMPLE_RATE_NTSC) == 0;
    if (s_audio) audio_set_playback_enabled(1);
    else fprintf(stderr, "[native] no audio device: playing silent\n");
    /* testing: BR_DEVICE_WAV=F, the engine's tap of the stream sent to the device (at the PSG rate,
       the FM and the PSG mixed, before the volume): tools/rewindsound.py reads a rewind's sound in it */
    if (const char *w = getenv("BR_DEVICE_WAV"); w && *w && s_audio) {
        if (audio_wav_start(w) != 0) fprintf(stderr, "[native] BR_DEVICE_WAV: cannot write %s\n", w);
        else atexit(audio_wav_stop);
    }
    s_period = (double)SDL_GetPerformanceFrequency() * 262.0 * 3420.0 / 53693175.0;   /* one NTSC frame */
    s_next = SDL_GetPerformanceCounter();
    return 1;
}

/* testing: BR_HOTKEY_TEST="N:F10,M:F9,..." presses that hotkey at the Nth input poll (played runs) */
static void hotkey_test(void)
{
    static const char *spec = getenv("BR_HOTKEY_TEST");
    static long polls;
    if (!spec) return;
    ++polls;
    for (const char *p = spec; p && *p; p = strchr(p, ',') ? strchr(p, ',') + 1 : nullptr) {
        char *e;
        long at = strtol(p, &e, 10);
        if (at != polls || *e != ':') continue;
        if (!strncmp(e + 1, "F10", 3)) options_toggle_widescreen();
        else if (!strncmp(e + 1, "F9", 2)) options_toggle_high_fps();
        else if (!strncmp(e + 1, "F4", 2) || !strncmp(e + 1, "F8", 2)) {
            /* the window's keys go through the event queue, as a press does (host_input's handler) */
            SDL_Event ev = {};
            ev.type = SDL_KEYDOWN;
            ev.key.keysym.scancode = e[2] == '4' ? SDL_SCANCODE_F4 : SDL_SCANCODE_F8;
            SDL_PushEvent(&ev);
        }
    }
}

/* Esc, the window closed, or a test's last shot: the picture line, then out (the runner's atexit
   lines follow) */
[[noreturn]] static void host_quit(void)
{
    picture_line();
    shader_close();
    if (s_audio) audio_close();
    SDL_Quit();
    exit(0);
}

void host_input(uint16_t pads[2])
{
    hotkey_test();
    s_pads_osd = s_win != nullptr;
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_ESCAPE))
            host_quit();
        if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_F11 && !e.key.repeat) {
            Uint32 f = SDL_GetWindowFlags(s_win);
            bool full = !(f & SDL_WINDOW_FULLSCREEN_DESKTOP);
            SDL_SetWindowFullscreen(s_win, full ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
            host_osd(full ? "FULLSCREEN" : "WINDOWED");
        }
        /* the presentation options' hotkeys (phase 6 R1; options.cpp) */
        if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_F9 && !e.key.repeat) options_toggle_high_fps();
        if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_F10 && !e.key.repeat) options_toggle_widescreen();
        /* quick saves (rewind.cpp, R33): F5 save, F6 the next slot, F7 load; Rewind held (controls.cpp,
           by default Backspace and a controller's left trigger) rewinds (below) */
        if (e.type == SDL_KEYDOWN && !e.key.repeat &&
            (e.key.keysym.scancode == SDL_SCANCODE_F5 || e.key.keysym.scancode == SDL_SCANCODE_F6 ||
             e.key.keysym.scancode == SDL_SCANCODE_F7)) {
            if (!rewind_on()) host_osd("REWIND AND SAVES ARE OFF");
            else if (e.key.keysym.scancode == SDL_SCANCODE_F5) rewind_key_save();
            else if (e.key.keysym.scancode == SDL_SCANCODE_F6) rewind_key_slot();
            else rewind_key_load();
        }
        /* the window's own (R32, R34) */
        if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_F4 && !e.key.repeat) {
            s_scaling_keys++;
            options_toggle_scaling();
        }
        if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_F8 && !e.key.repeat) shader_toggle();
        if (e.type == SDL_RENDER_DEVICE_RESET) make_renderer();
        pad_event(e);
    }
    pads[0] = controls_mask(0);
    pads[1] = controls_mask(1);
    if (rewind_on()) rewind_key_held(controls_rewind_held());
}

/* BR_PAD_SELFTEST=1: the slots with SDL's virtual game controllers, headless (no window, no ROM).
   A controller attached after SDL_Init is found by host_open's scan and then announced by
   SDL_CONTROLLERDEVICEADDED, as one plugged in before launch; unplugged, plugged in again, a second
   one, the first replugged, a joystick SDL has no mapping for. Each step's slots are printed; exit 0
   when every step holds. (X-Men 2's host_pad_selftest, for 2 players.) */
static int virtual_pad(bool generic = false)   /* its instance ID, or -1 */
{
    SDL_VirtualJoystickDesc d;
    SDL_zero(d);
    d.version = SDL_VIRTUAL_JOYSTICK_DESC_VERSION;
    d.type = generic ? SDL_JOYSTICK_TYPE_UNKNOWN : SDL_JOYSTICK_TYPE_GAMECONTROLLER;
    d.naxes = generic ? 2 : SDL_CONTROLLER_AXIS_MAX;
    d.nbuttons = generic ? 10 : SDL_CONTROLLER_BUTTON_MAX;
    d.nhats = generic ? 1 : 0;
    d.vendor_id = generic ? 0x1234 : 0;   /* its own GUID */
    d.product_id = generic ? 0x5678 : 0;
    d.name = generic ? "batmanrobin selftest generic pad" : "batmanrobin selftest pad";
    int index = SDL_JoystickAttachVirtualEx(&d);
    return index < 0 ? -1 : (int)SDL_JoystickGetDeviceInstanceID(index);
}

static SDL_Joystick *joy_of(int id)
{
    for (int p = 0; p < CTL_PLAYERS; p++)
        if (SDL_GameController *c = controls_player_pad(p))
            if (SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(c)) == id) return SDL_GameControllerGetJoystick(c);
    return SDL_JoystickFromInstanceID(id);
}

static void unplug(int id)
{
    for (int i = 0; i < SDL_NumJoysticks(); i++)
        if (SDL_JoystickGetDeviceInstanceID(i) == id) { SDL_JoystickDetachVirtual(i); return; }
}

static void pump(void)
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) pad_event(e);
}

int host_pad_selftest(void)
{
    SDL_SetMainReady();
    controls_sdl_hints();
    if (SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS) != 0) {
        fprintf(stderr, "[native] pad selftest: SDL: %s\n", SDL_GetError());
        return 2;
    }
    controls_load();
    for (int p = 0; p < CTL_PLAYERS; p++) controls_defaults(p);   /* not this folder's settings.ini */
    int steps = 0, good = 0;
    auto pad_id = [](SDL_GameController *c) { return (int)SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(c)); };
    auto expect = [&](const char *what, int p1, int p2) {   /* instance IDs in slots 1 and 2, -1 empty */
        int got[2];
        for (int n = 0; n < 2; n++) got[n] = controls_player_pad(n) ? pad_id(controls_player_pad(n)) : -1;
        bool ok = got[0] == p1 && got[1] == p2;
        steps++;
        good += ok;
        fprintf(stderr, "[native] pad selftest: %s: slots %d %d (want %d %d) %s\n", what, got[0], got[1], p1, p2,
                ok ? "ok" : "WRONG");
    };
    auto expect_pads = [&](const char *what, uint16_t w0, uint16_t w1) {
        uint16_t pads[2];
        host_input(pads);
        bool ok = pads[0] == w0 && pads[1] == w1;
        steps++;
        good += ok;
        fprintf(stderr, "[native] pad selftest: %s: pads %04x %04x (want %04x %04x) %s\n", what, pads[0], pads[1], w0, w1,
                ok ? "ok" : "WRONG");
    };
    int a = virtual_pad();
    if (a < 0) {
        fprintf(stderr, "[native] pad selftest: no virtual controller: %s\n", SDL_GetError());
        return 2;
    }
    controls_devices_init();   /* host_open's scan, then the event SDL queued for it */
    pump();
    expect("one controller before launch", a, -1);
    if (SDL_Joystick *j = joy_of(a)) {   /* its button reaches player 1 only */
        SDL_JoystickSetVirtualButton(j, SDL_CONTROLLER_BUTTON_A, 1);
        expect_pads("its south button (A): B, as before remapping", GPAD_B, 0);
        SDL_JoystickSetVirtualButton(j, SDL_CONTROLLER_BUTTON_A, 0);
        SDL_JoystickSetVirtualButton(j, SDL_CONTROLLER_BUTTON_X, 1);
        SDL_JoystickSetVirtualButton(j, SDL_CONTROLLER_BUTTON_B, 1);
        SDL_JoystickSetVirtualButton(j, SDL_CONTROLLER_BUTTON_START, 1);
        expect_pads("X, B and Start: A, C and Start, as before remapping", GPAD_A | GPAD_C | GPAD_START, 0);
        SDL_JoystickSetVirtualButton(j, SDL_CONTROLLER_BUTTON_X, 0);
        SDL_JoystickSetVirtualButton(j, SDL_CONTROLLER_BUTTON_B, 0);
        SDL_JoystickSetVirtualButton(j, SDL_CONTROLLER_BUTTON_START, 0);
        SDL_JoystickSetVirtualButton(j, SDL_CONTROLLER_BUTTON_A, 1);
        /* remapped: the south button is C, and the left stick (pushed right, past the dead zone) is nothing */
        PadIn south;
        south.kind = PadIn::BUTTON;
        south.code = SDL_CONTROLLER_BUTTON_A;
        controls_bind_pad(0, CB_C, 0, south);
        g_controls[0].pad[CB_RIGHT][1] = PadIn();
        SDL_JoystickSetVirtualAxis(j, SDL_CONTROLLER_AXIS_LEFTX, 30000);
        expect_pads("remapped: A = C, no stick", GPAD_C, 0);
        controls_defaults(0);
        expect_pads("the defaults again: A = B, stick right", GPAD_B | GPAD_RIGHT, 0);
        g_controls[0].deadzone = 95;
        expect_pads("a dead zone past the stick", GPAD_B, 0);
        controls_defaults(0);
        /* Rewind (rewind.cpp, R33): the left trigger by default, held; no pad bit; then unbound, not held */
        SDL_JoystickSetVirtualAxis(j, SDL_CONTROLLER_AXIS_TRIGGERLEFT, 30000);
        expect_pads("the left trigger: no game button", GPAD_B | GPAD_RIGHT, 0);
        bool rw = controls_rewind_held() && controls_rewind_held_by(0) && !controls_rewind_held_by(1);
        g_controls[0].pad[CB_REWIND][0] = PadIn();
        rw = rw && !controls_rewind_held();
        steps++;
        good += rw;
        fprintf(stderr, "[native] pad selftest: the left trigger is player 1's Rewind, unbound it is nothing: %s\n",
                rw ? "ok" : "WRONG");
        controls_defaults(0);
        SDL_JoystickSetVirtualAxis(j, SDL_CONTROLLER_AXIS_TRIGGERLEFT, 0);
        SDL_JoystickSetVirtualButton(j, SDL_CONTROLLER_BUTTON_A, 0);
        SDL_JoystickSetVirtualAxis(j, SDL_CONTROLLER_AXIS_LEFTX, 0);
        /* player 1 with no controller: it goes to player 2 */
        g_controls[0].device = "none";
        controls_reassign();
        expect("player 1 set to none", -1, a);
        g_controls[0].device = "auto";
        controls_reassign();
    }
    unplug(a);
    pump();
    expect("unplugged", -1, -1);
    int a2 = virtual_pad();
    pump();
    expect("plugged in again", a2, -1);
    int b = virtual_pad();
    pump();
    expect("a second controller", a2, b);
    unplug(a2);
    pump();
    expect("the first unplugged", -1, b);
    int a3 = virtual_pad();
    pump();
    expect("the first plugged in again", a3, b);
    /* player 2 asks for this model by GUID (all the virtual ones share it): it takes the first plugged in */
    char guid[64];
    SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joy_of(a3)), guid, sizeof guid);
    g_controls[1].device = guid;
    controls_reassign();
    expect("player 2 pinned to the model: the first plugged in (b)", a3, b);
    g_controls[1].device = "auto";
    controls_reassign();   /* a settings change: plug-in order */
    expect("player 2 auto again", b, a3);
    /* a joystick SDL has no mapping for: mapped here, player 2 once a3 is gone, its buttons in order and its hat */
    unplug(a3);
    pump();
    int g = virtual_pad(true);
    pump();
    SDL_GameController *gc = controls_player_pad(1);
    bool mapped = gc && pad_id(gc) == g;
    steps++;
    good += mapped;
    fprintf(stderr, "[native] pad selftest: a generic joystick is player 2: %s\n", mapped ? "ok" : "WRONG");
    if (mapped) {
        SDL_Joystick *j = SDL_GameControllerGetJoystick(gc);
        SDL_JoystickSetVirtualButton(j, 0, 1);
        SDL_JoystickSetVirtualButton(j, 9, 1);
        SDL_JoystickSetVirtualHat(j, 0, SDL_HAT_UP);
        uint16_t pads[2];
        host_input(pads);
        bool ok = pads[1] == (GPAD_B | GPAD_START | GPAD_UP) && !pads[0];
        steps++;
        good += ok;
        fprintf(stderr, "[native] pad selftest: its button 1, button 10 and hat up: player 2 %04x (want %04x) %s; labels %s, %s\n",
                pads[1], GPAD_B | GPAD_START | GPAD_UP, ok ? "ok" : "WRONG",
                controls_pad_label(g_controls[1].pad[CB_B][0], 1).c_str(), controls_pad_label(g_controls[1].pad[CB_START][0], 1).c_str());
    }
    int bad = controls_selftest();
    steps++;
    good += bad == 0;
    fprintf(stderr, "[native] pad selftest: %d of %d steps ok\n", good, steps);
    SDL_Quit();
    return good == steps ? 0 : 1;
}

/* [rewind] sound = reversed (rewind.cpp, R33): a rewind step's frame (frame C-1, played again after C)
   is heard backwards, so the steps C-1, C-2, ... are the sound before them backwards, end to end, with
   nothing stored. The FM is taken to the PSG rate first with audio_flush's own nearest-neighbour index
   (fi = i * fm_n / psg_n), so the two reversed at one rate mix to the forward sound exactly backwards
   (the kit's platforms/genesis/sound.md "Reversing the output"). A backwards frame is held back one
   frame: the last one, before the game plays on, the held picture's silence or a load, ends faded out,
   and the first sound after it starts faded in (an eighth of a frame, ~2 ms: no click where the wave
   turns round). From the X-Men 2 port's host_sdl.cpp (the same kit). */
namespace {
struct Rev {
    std::vector<int16_t> fm, psg;   /* fm: stereo pairs at the PSG rate */
    uint32_t frame = 0;
    bool held = false;      /* a backwards frame waits */
    bool fade_in = false;   /* the next sound starts faded in */
} s_rev;

void ramp(int16_t *b, size_t n, int ch, bool in)
{
    size_t k = n / 8;
    for (size_t i = 0; i < k; i++) {
        float g = ((float)i + 0.5f) / (float)k;
        size_t at = in ? i : n - 1 - i;
        for (int c = 0; c < ch; c++) b[at * ch + c] = (int16_t)((float)b[at * ch + c] * g);
    }
}

void rev_send(bool fade_out)
{
    if (!s_rev.held) return;
    s_rev.held = false;
    size_t n = s_rev.psg.size();
    if (fade_out) {
        ramp(s_rev.fm.data(), n, 2, false);
        ramp(s_rev.psg.data(), n, 1, false);
        s_rev.fade_in = true;
    }
    /* the FM already at the PSG rate: audio_flush's index is then i itself */
    if (s_audio) audio_flush(s_rev.frame, 1, s_rev.fm.data(), n, s_rev.psg.data(), n);
}
}  // namespace

void host_audio(uint64_t frame, int how)
{
    /* the frame's chip writes (the Z80's, stamped in master clocks from its frame's start), mixed,
       to the device (as the engine's sim step drains one NTSC wall frame); a rewind step's frame
       (how: rewind_audio()): the chips run, the device gets its sound backwards or silence */
    static int16_t fm[8192 * 2], psg[16384];
    size_t fm_n = 0, psg_n = 0;
    audio_mixer_drain(895780u, fm, 8192, &fm_n, psg, sizeof psg / sizeof psg[0], &psg_n);
    /* music_voices, fm_ladder, fm_filter: the FM is sound2.cpp's (both machines' chips) */
    if (snd_own_path()) fm_n = snd_mix(fm, 8192);
    bool silent = how == REWIND_AUDIO_SILENT, reversed = how == REWIND_AUDIO_REVERSED;
    if (silent) {
        memset(fm, 0, fm_n * 2 * sizeof fm[0]);
        memset(psg, 0, psg_n * sizeof psg[0]);
    }
    rev_send(!reversed);   /* a backwards frame before this one: as it is, or faded out when this one isn't */
    if (reversed && psg_n) {
        s_rev.fm.resize(psg_n * 2);
        for (size_t i = 0; i < psg_n; i++) {
            size_t fi = fm_n ? i * fm_n / psg_n : 0, at = psg_n - 1 - i;
            s_rev.fm[2 * at] = fm_n ? fm[2 * fi] : 0;
            s_rev.fm[2 * at + 1] = fm_n ? fm[2 * fi + 1] : 0;
        }
        s_rev.psg.assign(psg, psg + psg_n);
        std::reverse(s_rev.psg.begin(), s_rev.psg.end());
        if (s_rev.fade_in) {
            ramp(s_rev.fm.data(), psg_n, 2, true);
            ramp(s_rev.psg.data(), psg_n, 1, true);
        }
        s_rev.fade_in = false;
        s_rev.frame = (uint32_t)frame;
        s_rev.held = true;
        return;
    }
    if (s_rev.fade_in && !silent) {
        ramp(fm, fm_n, 2, true);
        ramp(psg, psg_n, 1, true);
    }
    s_rev.fade_in = silent;   /* forward after a silent stretch fades in too */
    if (s_audio) audio_flush((uint32_t)frame, 1, fm, fm_n, psg, psg_n);
}

/* rewind.cpp: a frame's length of silence (the picture held at the rewind's oldest frame) */
void host_audio_silence(void)
{
    static int16_t psg[3734];   /* one NTSC frame at the PSG rate (223,721 Hz / 59.92) */
    rev_send(true);
    s_rev.fade_in = true;
    if (s_audio) audio_flush(0, 1, nullptr, 0, psg, sizeof psg / sizeof psg[0]);
}

/* ... the sound queued for the device, dropped (a load: it was the other timeline's; a rewind's start) */
void host_audio_discard(void)
{
    s_rev.held = false;
    s_rev.fade_in = true;
    if (s_audio) audio_discard_playback();
}

/* The hotkeys' messages (F9, F10, F11), as the X-Men 2 port's: 8x8 text, white on a translucent
   black box, centred at the top of the game's picture for 2 s. Drawn on a copy at present time, so
   the game's frame (screenshots, dumps, --hash-frames) never has it. */
static char     s_osd[48];
static uint64_t s_osd_until;   /* SDL ticks (ms) */
static int      s_osd_new;     /* testing: the next picture drawn goes to $BR_OSD_DUMP */

void host_osd(const char *text)
{
    snprintf(s_osd, sizeof s_osd, "%s", text);
    s_osd_until = SDL_GetTicks64() + 2000;
    s_osd_new = 1;
}

/* testing: BR_OSD_DUMP=DIR writes the first picture drawn with each new message as DIR/osd_N.bmp */
static void osd_dump(const uint32_t *fb, int stride, int w, int h)
{
    static const char *dir = getenv("BR_OSD_DUMP");
    static int n;
    if (!dir || !s_osd_new) return;
    s_osd_new = 0;
    char path[1024];
    snprintf(path, sizeof path, "%s/osd_%d.bmp", dir, ++n);
    SDL_Surface *s = SDL_CreateRGBSurfaceWithFormatFrom((void *)fb, w, h, 32, stride * 4, SDL_PIXELFORMAT_ARGB8888);
    if (s) { SDL_SaveBMP(s, path); SDL_FreeSurface(s); }
}

static bool osd_live(int w, int h) { return s_osd[0] && SDL_GetTicks64() < s_osd_until && w <= 512 && h <= 480; }

/* the box and the text into buf (512 wide): over the picture copied there, or (layer: the shaded
   picture's, drawn over it after the chain) on a clear layer of their own, the box black at 2/3 alpha */
static void osd_paint(uint32_t *buf, int w, int h, bool layer)
{
    int n = (int)strlen(s_osd), tw = n * 8, x0 = (w - tw) / 2, y0 = 8;
    for (int y = y0 - 3; y < y0 + 11; y++)
        for (int x = x0 - 4; x < x0 + tw + 3; x++) {
            if (x < 0 || x >= w || y < 0 || y >= h) continue;
            uint32_t p = buf[y * 512 + x];   /* 2/3 black over the picture */
            buf[y * 512 + x] = layer ? 0xAA000000u
                                     : (p & 0xFF000000u) | ((p >> 16 & 0xFF) / 3) << 16 | ((p >> 8 & 0xFF) / 3) << 8 | (p & 0xFF) / 3;
        }
    for (int i = 0; i < n; i++) {
        unsigned char c = (unsigned char)s_osd[i];
        if (c < 0x20 || c > 0x7E) continue;
        for (int y = 0; y < 8; y++)
            for (int x = 0; x < 8; x++)
                if (k_osd_font[c - 0x20][y] >> x & 1) {
                    int px = x0 + i * 8 + x, py = y0 + y;
                    if (px >= 0 && px < w && py < h) buf[py * 512 + px] = 0xFFFFFFFFu;
                }
    }
}

static const uint32_t *osd_overlay(const uint32_t *fb, int *stride, int w, int h)
{
    static uint32_t buf[480 * 512];   /* the texture's size */
    if (!osd_live(w, h)) return fb;
    for (int y = 0; y < h; y++) memcpy(buf + y * 512, fb + y * *stride, (size_t)w * 4);
    osd_paint(buf, w, h, false);
    *stride = 512;
    return buf;
}

/* ... the layer over the shaded picture (shader.cpp): s_osd_tex at dst; false if there is no message */
static bool osd_layer(int w, int h)
{
    static uint32_t buf[480 * 512];
    if (!s_osd_tex || !osd_live(w, h)) return false;
    memset(buf, 0, sizeof buf);
    osd_paint(buf, w, h, true);
    SDL_Rect src = { 0, 0, w, h };
    return SDL_UpdateTexture(s_osd_tex, &src, buf, 512 * 4) == 0;
}

static void save_bmp(const char *path, void *pixels, int w, int h, int pitch)
{
    SDL_Surface *s = SDL_CreateRGBSurfaceWithFormatFrom(pixels, w, h, 32, pitch, SDL_PIXELFORMAT_ARGB8888);
    if (!s || SDL_SaveBMP(s, path) != 0) fprintf(stderr, "[native] %s: %s\n", path, SDL_GetError());
    if (s) SDL_FreeSurface(s);
}

/* testing: BR_BACKBUFFER_SHOT=N,M,... with BR_DUMP_DIR: at the Nth picture drawn, the window as presented
   (read back after the chain and the message: SDL_RenderReadPixels) and the picture it was drawn from;
   true once the last listed one is written (the run then quits) */
static bool backbuffer_shot(const uint32_t *fb, int stride, int w, int h, int ww, int wh)
{
    static const char *list = getenv("BR_BACKBUFFER_SHOT"), *dir = getenv("BR_DUMP_DIR");
    static long draws;
    if (!list || !dir) return false;
    ++draws;
    long last = 0;
    bool now = false;
    for (const char *q = list; *q; ) {
        char *e;
        long n = strtol(q, &e, 10);
        if (e == q) break;
        if (n > last) last = n;
        if (n == draws) now = true;
        q = *e == ',' ? e + 1 : e;
    }
    if (!now) return false;
    char path[1024];
    std::vector<uint32_t> px((size_t)ww * wh);
    snprintf(path, sizeof path, "%s/backbuffer_%ld.bmp", dir, draws);
    if (SDL_RenderReadPixels(s_ren, nullptr, SDL_PIXELFORMAT_ARGB8888, px.data(), ww * 4) != 0)
        fprintf(stderr, "[native] BR_BACKBUFFER_SHOT %s: %s\n", path, SDL_GetError());
    else
        save_bmp(path, px.data(), ww, wh, ww * 4);
    snprintf(path, sizeof path, "%s/frame_%ld.bmp", dir, draws);
    save_bmp(path, (void *)fb, w, h, stride * 4);
    return draws >= last;
}

/* The picture into the window (R32's scaling, R34's shader preset), the hotkeys' message over it */
static void draw(const uint32_t *fb, int stride, int w, int h)
{
    if (!s_ren || !s_tex || w <= 0 || h <= 0) return;
    int ww, wh;
    if (SDL_GetRendererOutputSize(s_ren, &ww, &wh) != 0 || ww <= 0 || wh <= 0) return;
    /* scaling: the largest size that fits; integer keeps a whole multiple of it (when one fits) */
    double sx = (double)ww / w, sy = (double)wh / h, s = sx < sy ? sx : sy;
    int mode = options_scaling(), k = (int)s;
    if (mode == SCALE_INTEGER && k >= 1) s = k;
    SDL_Rect dst = { (int)((ww - w * s) / 2), (int)((wh - h * s) / 2), (int)(w * s), (int)(h * s) };
    SDL_Rect src = { 0, 0, w, h };
    /* sharp_bilinear between whole multiples: nearest to k times into a target texture, then linear to the
       fitted size (each game pixel k screen pixels plus at most one blended one where it meets the next);
       at a whole multiple, or with no render targets, it is drawn as nearest */
    bool prescale = mode == SCALE_SHARP_BILINEAR && k >= 1 && s - k > 1e-6 && SDL_RenderTargetSupported(s_ren);
    if (prescale && (s_pre_w != w * k || s_pre_h != h * k)) {
        if (s_pre) SDL_DestroyTexture(s_pre);
        s_pre = SDL_CreateTexture(s_ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, w * k, h * k);
        s_pre_w = s_pre ? w * k : 0;
        s_pre_h = s_pre ? h * k : 0;
        if (s_pre) SDL_SetTextureScaleMode(s_pre, SDL_ScaleModeLinear);
        else fprintf(stderr, "[native] scaling=sharp_bilinear: no %dx%d target (%s), drawn sharp\n", w * k, h * k, SDL_GetError());
    }
    prescale = prescale && s_pre;
    SDL_SetRenderDrawColor(s_ren, 0, 0, 0, 255);
    SDL_RenderClear(s_ren);
    const uint32_t *shown = fb;
    int shown_stride = stride;
    if (shader_draw(s_ren, fb, stride, w, h, &dst)) {   /* shader_preset: the message over the shaded picture */
        if (osd_layer(w, h)) SDL_RenderCopy(s_ren, s_osd_tex, &src, &dst);
    } else {
        shown = osd_overlay(fb, &shown_stride, w, h);
        osd_dump(shown, shown_stride, w, h);
        SDL_UpdateTexture(s_tex, &src, shown, shown_stride * 4);
        if (prescale) {
            SDL_Rect whole = { 0, 0, w * k, h * k };
            SDL_SetRenderTarget(s_ren, s_pre);
            SDL_RenderCopy(s_ren, s_tex, &src, &whole);
            SDL_SetRenderTarget(s_ren, nullptr);
            SDL_RenderCopy(s_ren, s_pre, &whole, &dst);
        } else {
            SDL_RenderCopy(s_ren, s_tex, &src, &dst);
        }
    }
    bool last = backbuffer_shot(shown, shown_stride, w, h, ww, wh);
    SDL_RenderPresent(s_ren);
    if (last) host_quit();
}

static int s_at_started;     /* high_fps: the present clock has started */
static double s_at_base;     /* high_fps: performance-counter ticks at t_us 0 */

void host_present_clock_reset(void)
{
    s_at_started = 0;
    s_next = SDL_GetPerformanceCounter();
}

void host_present_at(const uint32_t *fb, int stride, int w, int h, double t_us)
{
    double f = (double)SDL_GetPerformanceFrequency();
    double now = (double)SDL_GetPerformanceCounter();
    if (!s_at_started) { s_at_base = now - t_us * f / 1e6; s_at_started = 1; }
    double due = s_at_base + t_us * f / 1e6;
    if (now > due + 4 * s_period) s_at_base += now - due;
    while ((now = (double)SDL_GetPerformanceCounter()) < due) {
        double left_ms = (due - now) * 1000.0 / f;
        if (left_ms > 2.0) SDL_Delay((Uint32)(left_ms - 1.5));
    }
    draw(fb, stride, w, h);
}

void host_present(const uint32_t *fb, int stride, int w, int h)
{
    draw(fb, stride, w, h);
    /* pace: one NTSC frame per frame; a late frame does not try to catch up */
    s_next += (uint64_t)s_period;
    uint64_t now = SDL_GetPerformanceCounter();
    if (now > s_next + (uint64_t)(4 * s_period)) s_next = now;
    while ((now = SDL_GetPerformanceCounter()) < s_next) {
        double left_ms = (double)(s_next - now) * 1000.0 / (double)SDL_GetPerformanceFrequency();
        if (left_ms > 2.0) SDL_Delay((Uint32)(left_ms - 1.5));
    }
}
