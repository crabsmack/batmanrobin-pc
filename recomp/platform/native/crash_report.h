/* crash_report.h - what a played session leaves when it stops (ruling R27, 2026-11-18).
 *
 * The native port stops on code it has no C for (P5 R8) and on a host exception; a player saw only
 * the window close (2026-11-17, after the crane boss). Now every played frame's pads go into a
 * log in the --input-script format (crash_log_frame; the F9/F10 changes as OPTION lines), and a
 * stop writes crash-reports/<time>/ beside the runner: report.txt (why, the C function, the frame,
 * the options, the replay command) and inputs.input (the session's pads from power-on), then says
 * so in a message box. The replay plays the same frames on the same machine: the game is
 * deterministic from its inputs.
 *
 * Test hooks: BR_CRASH_TEST=N stops at played or benchmark frame N as a fatal would (no message
 * box); BR_PLAY_SCRIPT=F (runner.cpp) feeds the played loop an input script's pads;
 * BR_INPUT_LOG=F writes the log to F when a --benchmark run ends (the format's round trip).
 */
#pragma once

#include <stdint.h>

void crash_report_install(const char *rom, int played);
void crash_log_frame(uint16_t p1, uint16_t p2);
void crash_log_option(const char *name, const char *value);
void crash_report_write(const char *reason);   /* writes the report (once) */
void crash_test_frame(uint64_t frame);         /* BR_CRASH_TEST */
void crash_log_finish(void);                    /* BR_INPUT_LOG at a benchmark run's end */
/* --record-inputs F (a played session): the same log written to F when the session ends, with the
   command that replays it (--benchmark ... --input-script F); the capture path's input (NOTES "The
   capture path"; kit playbook/release-video.md: footage is recorded inputs, not screen captures) */
void crash_log_record(const char *path);

/* quick saves and rewind (rewind.cpp): the log follows the timeline kept. Where it stands (before a
   frame is logged), back to such a place (a rewind step), the whole log (beside a quick save) and back */
#include <stddef.h>
#include <vector>
void crash_log_position(std::vector<uint8_t> &out);
void crash_log_back_to(const uint8_t *pos, size_t len);
void crash_log_copy(std::vector<uint8_t> &out);
void crash_log_paste(const std::vector<uint8_t> &in);
