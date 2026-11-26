/* crash_report.cpp - the session's input log and the report a stop leaves (crash_report.h). */
#include "crash_report.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <map>
#include <string>
#include <vector>

#include <SDL.h>

#include "host_sdl.h"
#include "options.h"

#ifdef _WIN32
#include <direct.h>
#define mkdir_one(p) _mkdir(p)
#else
#include <sys/stat.h>
#define mkdir_one(p) mkdir(p, 0755)
#endif

namespace dk {
extern char g_fatal_msg[512];
extern uint32_t g_fn_current;
extern void (*g_fatal_hook)(void);
}
extern "C" void dk_install_crash_report(void (*report)(unsigned long code, void *addr));

namespace {

std::string s_rom = "ROM";        /* the cartridge's file name (no folders: a report may be shared) */
std::string s_first_options;      /* the options the session started with */
std::vector<std::string> s_lines; /* the log so far, PADS / WAIT / OPTION */
uint16_t s_run_p1, s_run_p2;
uint64_t s_run_n, s_frames;
int s_played, s_written;
uint64_t s_test_frame;

std::string hex4(uint16_t v)
{
    char b[8];
    snprintf(b, sizeof b, "%03X", (unsigned)v);
    return b;
}

void flush_run(void)
{
    if (!s_run_n) return;
    s_lines.push_back("PADS " + hex4(s_run_p1) + " " + hex4(s_run_p2));
    s_lines.push_back("WAIT " + std::to_string(s_run_n));
    s_run_n = 0;
}

/* options_effective_json as K=V,... (the --options form) */
std::string options_cli(void)
{
    std::string j = options_effective_json(), o;
    size_t p = 0;
    while ((p = j.find('"', p)) != std::string::npos) {
        size_t e = j.find('"', p + 1);
        size_t v0 = j.find('"', e + 1), v1 = v0 == std::string::npos ? v0 : j.find('"', v0 + 1);
        if (e == std::string::npos || v1 == std::string::npos) break;
        o += (o.empty() ? "" : ",") + j.substr(p + 1, e - p - 1) + "=" + j.substr(v0 + 1, v1 - v0 - 1);
        p = v1 + 1;
    }
    return o;
}

std::string exe_dir(void)
{
    char *b = SDL_GetBasePath();
    std::string d = b ? b : "./";
    SDL_free(b);
    return d;
}

std::string write_log(const std::string &path, const std::string &replay)
{
    flush_run();
    FILE *f = fopen(path.c_str(), "w");
    if (!f) return "";
    fprintf(f, "# The pads of a played session from power-on, one PADS per change (P1 P2, GPAD_* bits),\n"
               "# held for WAIT frames; OPTION lines are the F9/F10 changes where they happened.\n");
    if (!replay.empty()) fprintf(f, "# replay: %s\n", replay.c_str());
    for (const std::string &l : s_lines) fprintf(f, "%s\n", l.c_str());
    fclose(f);
    return path;
}

void on_fatal(void)
{
    char why[700];
    snprintf(why, sizeof why, "%s (C function $%06X)", dk::g_fatal_msg, (unsigned)dk::g_fn_current);
    crash_report_write(why);
}

void on_exception(unsigned long code, void *addr)
{
    char why[160];
    snprintf(why, sizeof why, "host exception %08lX at %p (C function $%06X)", code, addr,
             (unsigned)dk::g_fn_current);
    crash_report_write(why);
}

}  // namespace

void crash_report_install(const char *rom, int played)
{
    std::string r = rom ? rom : "ROM";
    size_t cut = r.find_last_of("/\\");
    s_rom = cut == std::string::npos ? r : r.substr(cut + 1);
    s_first_options = options_cli();
    s_played = played;
    dk::g_fatal_hook = on_fatal;
    dk_install_crash_report(on_exception);
    if (const char *t = getenv("BR_CRASH_TEST")) s_test_frame = strtoull(t, nullptr, 0);
}

void crash_log_frame(uint16_t p1, uint16_t p2)
{
    s_frames++;
    if (s_run_n && p1 == s_run_p1 && p2 == s_run_p2) { s_run_n++; return; }
    flush_run();
    s_run_p1 = p1, s_run_p2 = p2, s_run_n = 1;
}

void crash_log_option(const char *name, const char *value)
{
    flush_run();
    s_lines.push_back(std::string("OPTION ") + name + " " + value);
}

/* Quick saves and rewind (rewind.cpp, R33): the log follows the timeline kept, so a report's replay
   still reaches the frame it stopped at. A rewind step goes back to where the log stood when the
   replayed frame began (lines are only appended: a place is their count and the run being counted);
   a quick load puts back the log saved with the slot. Either way the options in effect now (an F9/F10
   pressed meanwhile) are written after it where the log would say otherwise. */
namespace {

struct LogPos { uint64_t lines, run_n, frames; uint16_t p1, p2; };

std::map<std::string, std::string> parse_kv(const std::string &cli)
{
    std::map<std::string, std::string> m;
    for (size_t p = 0; p < cli.size();) {
        size_t c = cli.find(',', p);
        if (c == std::string::npos) c = cli.size();
        std::string kv = cli.substr(p, c - p);
        size_t eq = kv.find('=');
        if (eq != std::string::npos) m[kv.substr(0, eq)] = kv.substr(eq + 1);
        p = c + 1;
    }
    return m;
}

void options_resync(void)
{
    std::map<std::string, std::string> said = parse_kv(s_first_options);
    for (const std::string &l : s_lines)
        if (l.compare(0, 7, "OPTION ") == 0) {
            size_t sp = l.find(' ', 7);
            if (sp != std::string::npos) said[l.substr(7, sp - 7)] = l.substr(sp + 1);
        }
    for (const auto &kv : parse_kv(options_cli()))
        if (said.count(kv.first) && said[kv.first] != kv.second) crash_log_option(kv.first.c_str(), kv.second.c_str());
}

}  // namespace

void crash_log_position(std::vector<uint8_t> &out)
{
    LogPos p;
    memset(&p, 0, sizeof p);
    p.lines = s_lines.size();
    p.run_n = s_run_n;
    p.frames = s_frames;
    p.p1 = s_run_p1;
    p.p2 = s_run_p2;
    out.assign((const uint8_t *)&p, (const uint8_t *)&p + sizeof p);
}

void crash_log_back_to(const uint8_t *pos, size_t len)
{
    LogPos p;
    if (len != sizeof p) return;
    memcpy(&p, pos, sizeof p);
    if (p.lines > s_lines.size()) return;
    s_lines.resize((size_t)p.lines);
    s_run_n = p.run_n;
    s_frames = p.frames;
    s_run_p1 = p.p1;
    s_run_p2 = p.p2;
    options_resync();
}

void crash_log_copy(std::vector<uint8_t> &out)
{
    crash_log_position(out);
    for (const std::string &l : s_lines) {
        out.insert(out.end(), l.begin(), l.end());
        out.push_back('\n');
    }
}

void crash_log_paste(const std::vector<uint8_t> &in)
{
    if (in.size() < sizeof(LogPos)) return;
    LogPos p;
    memcpy(&p, in.data(), sizeof p);
    s_lines.clear();
    std::string l;
    for (size_t i = sizeof p; i < in.size(); i++) {
        if (in[i] == '\n') { s_lines.push_back(l); l.clear(); }
        else l += (char)in[i];
    }
    s_run_n = p.run_n;
    s_frames = p.frames;
    s_run_p1 = p.p1;
    s_run_p2 = p.p2;
    options_resync();
}

void crash_test_frame(uint64_t frame)
{
    if (s_test_frame && frame == s_test_frame) {
        snprintf(dk::g_fatal_msg, sizeof dk::g_fatal_msg, "crash test at frame %llu (BR_CRASH_TEST)",
                 (unsigned long long)frame);
        fprintf(stderr, "[native] %s\n", dk::g_fatal_msg);
        on_fatal();
        exit(5);
    }
}

void crash_log_finish(void)
{
    if (const char *p = getenv("BR_INPUT_LOG")) write_log(p, "");
}

static std::string s_record;   /* --record-inputs */

static void record_at_exit(void)
{
    if (s_record.empty()) return;
    std::string name = s_record;
    size_t cut = name.find_last_of("/\\");
    if (cut != std::string::npos) name = name.substr(cut + 1);
    std::string replay = "batmanrobin_native \"" + s_rom + "\" --benchmark " + std::to_string(s_frames) +
                         " --input-script " + name + " --options " + s_first_options;
    if (write_log(s_record, replay).empty()) fprintf(stderr, "[native] cannot write %s\n", s_record.c_str());
    else fprintf(stderr, "[native] %llu frames of inputs in %s\n", (unsigned long long)s_frames, s_record.c_str());
}

void crash_log_record(const char *path)
{
    if (s_record.empty()) atexit(record_at_exit);
    s_record = path ? path : "";
}

void crash_report_write(const char *reason)
{
    if (s_written) return;
    s_written = 1;
    time_t now = time(nullptr);
    struct tm tmv;
#ifdef _WIN32
    localtime_s(&tmv, &now);
#else
    localtime_r(&now, &tmv);
#endif
    char stamp[32];
    strftime(stamp, sizeof stamp, "%Y%m%d-%H%M%S", &tmv);
    std::string base = exe_dir() + "crash-reports";
    mkdir_one(base.c_str());
    std::string dir = base + "/" + stamp;
    mkdir_one(dir.c_str());

    std::string replay = "batmanrobin_native \"" + s_rom + "\" --benchmark " + std::to_string(s_frames + 600) +
                         " --input-script inputs.input --options " + s_first_options;
    std::string log = write_log(dir + "/inputs.input", replay);
    std::string rep = dir + "/report.txt";
    if (FILE *f = fopen(rep.c_str(), "w")) {
        fprintf(f, "The Adventures of Batman and Robin (native port, %s) stopped.\n\n", GENESIS_GAME_VERSION);
        fprintf(f, "Why:      %s\n", reason);
        fprintf(f, "Frame:    %llu frames since power-on\n", (unsigned long long)s_frames);
        fprintf(f, "Options:  %s (at the stop)\n", options_cli().c_str());
        fprintf(f, "Started:  %s\n", s_first_options.c_str());
        fprintf(f, "Inputs:   %s\n\n", log.empty() ? "(could not be written)" : "inputs.input");
        fprintf(f, "\"no C for code address\" means the game reached code the port has not translated yet:\n"
                   "please send this folder with the issue. The replay plays the same session on the\n"
                   "same build, from the folder holding the runner:\n  %s\n", replay.c_str());
        fclose(f);
    }
    fprintf(stderr, "[native] stopped: %s; report in %s\n", reason, dir.c_str());
    fflush(stderr);
    if (s_played && !s_test_frame) {   /* (a test's stop shows no box: nobody is there to close it) */
        std::string msg = std::string("The game stopped:\n") + reason + "\n\nA report and the session's inputs "
                          "were saved in\n" + dir;
        host_message_error(msg.c_str());   /* printed instead where nobody can close it (host_sdl.cpp) */
    }
}
