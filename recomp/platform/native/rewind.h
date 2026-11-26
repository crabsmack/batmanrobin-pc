/* rewind.h - quick saves and rewind (rewind.cpp; docs/phases/P6-enhance.md R33). */
#pragma once

#include <stdint.h>
#include <string>
#include <vector>

/* the runner (runner.cpp): set up after nm_power_on; on in a played run, or headless with
   BR_REWIND_KEYS (the tests) */
void rewind_init(int played);
int  rewind_on(void);

/* The end of a host frame (the game's fiber parked): `ran` = a frame was run (not a held picture).
   Pushes it into the ring, or checks a rewind step's replay; then the keys: save, load, slot, a
   rewind step. RW_FORWARD: run the next frame as usual; RW_STEP: a state was loaded and the next
   frame is a replay (its input set through main_replay_input); RW_HOLD: held at the ring's oldest
   frame: show the same picture again, run nothing. A load moves the frame number (nm_frame_no). */
enum { RW_FORWARD, RW_STEP, RW_HOLD };
int  rewind_after_frame(bool ran);
int  rewind_loaded(void);   /* the last rewind_after_frame loaded a state (a step or a quick load) */

enum { REWIND_AUDIO_FORWARD, REWIND_AUDIO_SILENT, REWIND_AUDIO_REVERSED };
int  rewind_audio(void);    /* the frame just run: forward, or a rewind step's (silent or backwards) */

/* the keys (host_sdl.cpp): taken at the next frame's end */
void rewind_key_save(void);
void rewind_key_load(void);
void rewind_key_slot(void);
void rewind_key_held(bool held);

std::string rewind_counters_json(void);
int  rewind_selftest(void);   /* BR_REWIND_SELFTEST=1: the encoding; 0 = pass */

/* runner.cpp's side: the input a frame began with (played: the pads and where the session's input
   log stood; headless: where the input script stood), given back for a replay; the runner's own
   timeline beside a quick save (the input log; the script's place) */
void main_frame_input(std::vector<uint8_t> &out);
void main_replay_input(const std::vector<uint8_t> &in);
void main_timeline_save(std::vector<uint8_t> &out);
void main_timeline_load(const std::vector<uint8_t> &in);
