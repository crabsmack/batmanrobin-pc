/* host_sdl.h - the native runner's window, input and sound when played (host_sdl.cpp). */
#ifndef NATIVE_HOST_SDL_H
#define NATIVE_HOST_SDL_H

#include <stdint.h>

int  host_open(const char *title);    /* 0: no display */
void host_input(uint16_t pads[2]);    /* GPAD_* masks of players 1-2; quits on Esc / close */
void host_audio(uint64_t frame, int how = 0);   /* the frame's sound, mixed, to the device (how: rewind_audio(),
                                                   a rewind step's frame heard backwards or silent) */
void host_audio_silence(void);   /* rewind.cpp: one frame's length of silence (the picture held) */
void host_audio_discard(void);   /* rewind.cpp: the sound queued for the device dropped (a load) */
void host_present(const uint32_t *fb, int stride, int w, int h);   /* the picture, then paced to NTSC */
/* high_fps (phase 6 R3): the picture at its time on the present clock (us from the clock's start,
   the first call); a frame more than 4 NTSC frames late moves the clock instead of hurrying */
void host_present_at(const uint32_t *fb, int stride, int w, int h, double t_us);
void host_present_clock_reset(void);   /* high_fps turned on or off: both clocks start again from now */
void host_osd(const char *text);       /* a hotkey's message on the picture for 2 s (up to 47 characters) */

/* the front door (runner.cpp) */
#include <stddef.h>
void host_detach_lone_console(void);   /* Windows, played: close the console window a double-click opened */
void host_messages_print(int on);      /* --no-messagebox: every message box printed to stderr instead */
int  host_messages_printed(void);      /* 1: no dialog may open (the switch, BR_NO_MESSAGEBOX, headless) */
void host_message(const char *text);   /* a message box (before or without the window), or printed */
void host_message_error(const char *text);
bool host_pick_rom(char *path, size_t cap);   /* a file picker for the ROM; false: cancelled, none, or headless */
bool host_pick_shader(char *path, size_t cap);   /* ... for a shader preset (.slangp; R34, the launcher) */
int  host_pad_selftest(void);          /* BR_PAD_SELFTEST: the controllers' slots with virtual ones; 0 = pass */

#endif /* NATIVE_HOST_SDL_H */
