/* host_sdl.cpp - the native runner's window, input and sound when played (phase 5 P6/P8). SDL2 for
 * the window, the keyboard, game controllers and the audio device; the sound chips and their mixer
 * are the engine's (audio/: ymfm's YM2612, the SN76489, the cycle-stamped event queue the Z80's and
 * the PSG's writes go into, audio.c's device bridge).
 *
 *   Keyboard (player 1): arrows; Z = A, X = B, C = C, Enter = Start.
 *   Game controllers (players 1-2 in plug-in order, the keyboard with the first): D-pad or left
 *   stick; X = A, A = B, B = C, Start.  F11 toggles fullscreen; Esc or closing the window quits.
 *   F9 turns high_fps off and on, F10 the 16:9 view (phase 6's presentation options, options.cpp).
 * Frames are paced to the NTSC rate (59.92 Hz) by the clock.
 *
 * Adapted from the X-Men 2 port's platform/native/host_sdl.cpp (the same kit).
 */
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern "C" {
#include "audio.h"
#include "audio/mixer.h"
#include "genesis_clocks.h"
#include "video/genesis_bus.h"
}
#include "host_sdl.h"
#include "options.h"
#include "sound2.h"

static SDL_Window *s_win;
static SDL_Renderer *s_ren;
static SDL_Texture *s_tex;
static SDL_GameController *s_pad[2];
static uint64_t s_next;
static double s_period;
static int s_audio;

static int make_renderer(void)
{
    if (s_tex) { SDL_DestroyTexture(s_tex); s_tex = nullptr; }
    if (s_ren) { SDL_DestroyRenderer(s_ren); s_ren = nullptr; }
#ifdef _WIN32
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d11");
    s_ren = SDL_CreateRenderer(s_win, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "");
#endif
    if (!s_ren) s_ren = SDL_CreateRenderer(s_win, -1, SDL_RENDERER_ACCELERATED);
    if (!s_ren) s_ren = SDL_CreateRenderer(s_win, -1, 0);
    if (!s_ren) { fprintf(stderr, "[native] SDL renderer: %s\n", SDL_GetError()); return 0; }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    s_tex = SDL_CreateTexture(s_ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 512, 480);
    if (!s_tex) { fprintf(stderr, "[native] SDL texture: %s\n", SDL_GetError()); return 0; }
    return 1;
}

static void pad_add(int index)
{
    if (!SDL_IsGameController(index)) return;
    SDL_JoystickID id = SDL_JoystickGetDeviceInstanceID(index);
    for (int n = 0; n < 2; n++)
        if (s_pad[n] && SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(s_pad[n])) == id) return;
    for (int n = 0; n < 2; n++)
        if (!s_pad[n]) {
            s_pad[n] = SDL_GameControllerOpen(index);
            return;
        }
}

static void pad_remove(SDL_JoystickID id)
{
    for (int n = 0; n < 2; n++)
        if (s_pad[n] && SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(s_pad[n])) == id) {
            SDL_GameControllerClose(s_pad[n]);
            s_pad[n] = nullptr;
        }
}

int host_open(const char *title)
{
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS) != 0) {
        fprintf(stderr, "[native] SDL: %s\n", SDL_GetError());
        return 0;
    }
    s_win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320 * 3, 224 * 3,
                             SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!s_win) { fprintf(stderr, "[native] SDL window: %s\n", SDL_GetError()); return 0; }
    if (!make_renderer()) return 0;
    for (int i = 0; i < SDL_NumJoysticks(); i++) pad_add(i);
    audio_mixer_init();
    s_audio = audio_init((int)GENESIS_PSG_SAMPLE_RATE_NTSC) == 0;
    if (s_audio) audio_set_playback_enabled(1);
    else fprintf(stderr, "[native] no audio device: playing silent\n");
    s_period = (double)SDL_GetPerformanceFrequency() * 262.0 * 3420.0 / 53693175.0;   /* one NTSC frame */
    s_next = SDL_GetPerformanceCounter();
    return 1;
}

static uint16_t keyboard_pad(void)
{
    const Uint8 *k = SDL_GetKeyboardState(nullptr);
    uint16_t p = 0;
    if (k[SDL_SCANCODE_UP]) p |= GPAD_UP;
    if (k[SDL_SCANCODE_DOWN]) p |= GPAD_DOWN;
    if (k[SDL_SCANCODE_LEFT]) p |= GPAD_LEFT;
    if (k[SDL_SCANCODE_RIGHT]) p |= GPAD_RIGHT;
    if (k[SDL_SCANCODE_Z]) p |= GPAD_A;
    if (k[SDL_SCANCODE_X]) p |= GPAD_B;
    if (k[SDL_SCANCODE_C]) p |= GPAD_C;
    if (k[SDL_SCANCODE_RETURN]) p |= GPAD_START;
    return p;
}

static uint16_t controller_pad(SDL_GameController *c)
{
    if (!c) return 0;
    uint16_t p = 0;
    const int dead = 12000;
    int x = SDL_GameControllerGetAxis(c, SDL_CONTROLLER_AXIS_LEFTX), y = SDL_GameControllerGetAxis(c, SDL_CONTROLLER_AXIS_LEFTY);
    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_DPAD_UP) || y < -dead) p |= GPAD_UP;
    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_DPAD_DOWN) || y > dead) p |= GPAD_DOWN;
    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_DPAD_LEFT) || x < -dead) p |= GPAD_LEFT;
    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) || x > dead) p |= GPAD_RIGHT;
    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_X)) p |= GPAD_A;
    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_A)) p |= GPAD_B;
    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_B)) p |= GPAD_C;
    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_START)) p |= GPAD_START;
    return p;
}

void host_input(uint16_t pads[2])
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)) {
            if (s_audio) audio_close();
            SDL_Quit();
            exit(0);
        }
        if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_F11 && !e.key.repeat) {
            Uint32 f = SDL_GetWindowFlags(s_win);
            SDL_SetWindowFullscreen(s_win, (f & SDL_WINDOW_FULLSCREEN_DESKTOP) ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
        }
        /* the presentation options' hotkeys (phase 6 R1; options.cpp) */
        if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_F9 && !e.key.repeat) options_toggle_high_fps();
        if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_F10 && !e.key.repeat) options_toggle_widescreen();
        if (e.type == SDL_RENDER_DEVICE_RESET) make_renderer();
        if (e.type == SDL_CONTROLLERDEVICEADDED) pad_add(e.cdevice.which);
        else if (e.type == SDL_CONTROLLERDEVICEREMOVED) pad_remove(e.cdevice.which);
    }
    pads[0] = (uint16_t)(keyboard_pad() | controller_pad(s_pad[0]));
    pads[1] = controller_pad(s_pad[1]);
}

void host_audio(uint64_t frame)
{
    /* the frame's chip writes (the Z80's, stamped in master clocks from its frame's start), mixed,
       to the device (as the engine's sim step drains one NTSC wall frame) */
    static int16_t fm[8192 * 2], psg[16384];
    size_t fm_n = 0, psg_n = 0;
    audio_mixer_drain(895780u, fm, 8192, &fm_n, psg, sizeof psg / sizeof psg[0], &psg_n);
    /* music_voices, fm_ladder, fm_filter: the FM is sound2.cpp's (both machines' chips) */
    if (snd_own_path()) fm_n = snd_mix(fm, 8192);
    if (s_audio) audio_flush((uint32_t)frame, 1, fm, fm_n, psg, psg_n);
}

static void draw(const uint32_t *fb, int stride, int w, int h)
{
    if (s_ren && s_tex && w > 0 && h > 0) {
        SDL_Rect src = { 0, 0, w, h };
        SDL_UpdateTexture(s_tex, &src, fb, stride * 4);
        int ww, wh;
        if (SDL_GetRendererOutputSize(s_ren, &ww, &wh) == 0 && ww > 0 && wh > 0) {
            double sx = (double)ww / w, sy = (double)wh / h, sc = sx < sy ? sx : sy;
            SDL_Rect dst = { (int)((ww - w * sc) / 2), (int)((wh - h * sc) / 2), (int)(w * sc), (int)(h * sc) };
            SDL_SetRenderDrawColor(s_ren, 0, 0, 0, 255);
            SDL_RenderClear(s_ren);
            SDL_RenderCopy(s_ren, s_tex, &src, &dst);
            SDL_RenderPresent(s_ren);
        }
    }
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
