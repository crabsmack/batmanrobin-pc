/* capture.cpp - the port's pictures to a lossless video, for looking at frame by frame or for a
 * release video (docs/NOTES.md "The capture path"; kit playbook/release-video.md: footage is
 * recorded inputs replayed at any setting, not screen captures). Nothing the game sees.
 *
 *   BR_CAPTURE=F       every runner frame's picture (a played session's or a --benchmark replay's:
 *                      a replay renders unless --no-render), FFV1 in
 *                      Matroska at the console's rate (53693175/896040 Hz), through an ffmpeg on
 *                      PATH, on a fixed canvas: BR_CAPTURE_W (default 400 with widescreen on at
 *                      the start, else 320) x 224, narrower pictures (the 4:3 screens in 16:9)
 *                      centred on black.
 *   BR_WAV=PREFIX      (sound2.cpp) the sound as PREFIX.mix.wav, .music.wav and .sfx.wav; a
 *                      headless replay needs --audio-hash for it.
 *
 * With --record-inputs F (crash_report.h) a played session leaves the inputs that replay it:
 *   batmanrobin_native ROM --record-inputs take.input                       (play)
 *   set BR_CAPTURE=take.mkv & set BR_WAV=take
 *   batmanrobin_native ROM --benchmark N --input-script take.input --audio-hash --options ...
 * (N and the options: the "# replay:" line the take starts with)
 * then any options (16:9, unlimited_sprites) render the same take again.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <string>
#include <vector>

#include "capture.h"
#include "options.h"

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

static FILE *s_pipe;
static int s_w, s_h = 224;
static std::vector<uint32_t> s_canvas;
static uint64_t s_frames;

static void capture_close(void)
{
    if (!s_pipe) return;
    pclose(s_pipe);
    s_pipe = nullptr;
    fprintf(stderr, "[capture] %llu frames written\n", (unsigned long long)s_frames);
}

void capture_init(void)
{
    const char *path = getenv("BR_CAPTURE");
    if (!path || !*path) return;
    const char *w = getenv("BR_CAPTURE_W");
    s_w = w && *w ? atoi(w) : (g_opt.widescreen ? 400 : 320);
    if (s_w < 256 || s_w > 512) s_w = 320;
    s_canvas.assign((size_t)s_w * s_h, 0xFF000000u);
    std::string cmd = "ffmpeg -loglevel error -y -f rawvideo -pix_fmt bgra -s " + std::to_string(s_w) + "x" +
                      std::to_string(s_h) + " -r 53693175/896040 -i - -c:v ffv1 -level 3 -pix_fmt bgr0 \"" +
                      std::string(path) + "\"";
    s_pipe = popen(cmd.c_str(), "wb");
    if (!s_pipe) { fprintf(stderr, "[capture] cannot start ffmpeg (BR_CAPTURE): %s\n", cmd.c_str()); exit(2); }
    atexit(capture_close);
}

void capture_frame(const uint32_t *fb, int stride, int w, int h)
{
    if (!s_pipe) return;
    int ww = w < s_w ? w : s_w, hh = h < s_h ? h : s_h;
    int x0 = (s_w - ww) / 2, sx = (w - ww) / 2;
    std::fill(s_canvas.begin(), s_canvas.end(), 0xFF000000u);
    for (int y = 0; y < hh; y++)
        memcpy(&s_canvas[(size_t)y * s_w + x0], fb + (size_t)y * stride + sx, (size_t)ww * 4);
    for (size_t i = 0; i < s_canvas.size(); i++) s_canvas[i] |= 0xFF000000u;
    if (fwrite(s_canvas.data(), 4, s_canvas.size(), s_pipe) != s_canvas.size()) {
        fprintf(stderr, "[capture] ffmpeg stopped taking frames\n");
        pclose(s_pipe);
        s_pipe = nullptr;
        return;
    }
    s_frames++;
}
