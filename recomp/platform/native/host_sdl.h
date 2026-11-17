/* host_sdl.h - the native runner's window, input and sound when played (host_sdl.cpp). */
#ifndef NATIVE_HOST_SDL_H
#define NATIVE_HOST_SDL_H

#include <stdint.h>

int  host_open(const char *title);    /* 0: no display */
void host_input(uint16_t pads[2]);    /* GPAD_* masks of players 1-2; quits on Esc / close */
void host_audio(uint64_t frame);      /* the frame's sound, mixed, to the device */
void host_present(const uint32_t *fb, int stride, int w, int h);   /* the picture, then paced to NTSC */
/* high_fps (phase 6 R3): the picture at its time on the present clock (us from the clock's start,
   the first call); a frame more than 4 NTSC frames late moves the clock instead of hurrying */
void host_present_at(const uint32_t *fb, int stride, int w, int h, double t_us);
void host_present_clock_reset(void);   /* high_fps turned on or off: both clocks start again from now */

#endif /* NATIVE_HOST_SDL_H */
