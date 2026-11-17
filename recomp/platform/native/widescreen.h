/* widescreen.h - the native runner's 16:9 view (docs/phases/P6-enhance.md E3, rulings R4, R5, R11,
 * R12, R16; docs/NOTES.md "Phase 6: widescreen"): what the machine calls around each drawn frame,
 * what the game's native branches call, and the render camera and margin state the present path
 * (hfps-p6's interpolator) reads.
 *
 * Nothing here is game state: the view, the render camera's shift and the counters are the host's.
 * What the game's native branches report (the scene header a scene started from, the whole-drawn
 * plane's drawn cells) is kept in WsSaved, which the machine's branch snapshot carries.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WS_MARGIN 40               /* px per side: 320 + 2 x 40 = 400 */

/* The view of the frame being drawn (set by ws_frame_setup, kept until the next). */
typedef struct WsView {
    int wide;                      /* 1: this screen is drawn 16:9 (the renderer has margins) */
    int margin;                    /* px per side (0 on a 4:3 screen) */
    int shift;                     /* the render camera: the window starts at camera - margin + shift */
    int bar_l, bar_r;              /* black columns at the frame's edges (past the map's ends), the
                                      pinned HUD drawn over them (g_ws_bars) */
    int centre_x0;                 /* frame x of the 4:3 view's x 0 (margin - shift) */
    int gameplay;                  /* a gameplay frame (enh_gameplay) */
} WsView;
extern WsView g_ws_view;

/* The bars of the frame being drawn and the pinned HUD's pixels over them (widescreen.cpp "the HUD
   over the bars"): ws_frame_check paints them over the machine's frame, high_fps's present path
   (present.cpp) over each interpolated one, both through ws_bars_paint. */
#define WS_BAR_MAX  (2 * WS_MARGIN)   /* a bar's widest: the margin and the render camera's shift */
#define WS_BAR_ROWS 240
typedef struct WsBars {
    int bar_l, bar_r;              /* = g_ws_view's */
    int w;                         /* the frame width keep[] is for (320 + 2 x WS_MARGIN) */
    int nkeep;                     /* pixels kept this frame */
    uint8_t keep[WS_BAR_ROWS][2][WS_BAR_MAX];   /* [line][0 left: frame x, 1 right: w - 1 - x]: a pinned
                                                   HUD piece's opaque pixel, drawn over the bar */
} WsBars;
extern WsBars g_ws_bars;
void ws_bars_paint(const WsBars *b, uint32_t *fb, int stride, int w, int h);

/* The machine (machine.cpp): around the display of each drawn frame, and per line after the H-int */
void ws_frame_setup(void);
void ws_line(int line);
void ws_frame_check(void);
void ws_power_on(void);

/* The drawn frame's window (ws_frame_setup .. ws_frame_check): the game's own VRAM writes inside it
   (an H-int's, a DMA it starts) are reported byte by byte, so what the view wrote for the frame is
   put back only where the game did not write after it (machine.cpp's VRAM write paths). */
extern int g_ws_window;
void ws_vram_touch(uint16_t addr);
uint8_t ws_vram_game8(uint16_t addr);   /* a VRAM byte as the game left it (its reads and copies) */

/* high_fps's capture (present.cpp, after ws_line): the R12 margin columns set for this line, in
   GVDP_WS_VCOL_FIRST..LAST order (set[k]: column FIRST + k filled, val[k]: its plane A, B words;
   the engine keeps them outside the VDP struct). Returns the column count (0: none filled). */
int ws_margin_columns(const int **set, const uint16_t (**val)[2]);

/* The host sprite layer's line (ws_draw.cpp draw_line, after the margin pieces): the stage 1
   street's plane B continued into the margins (widescreen.cpp "the street") */
struct GVDP;
struct GVDPSpriteLayer;
void ws_street_line(const struct GVDP *v, const struct GVDPSpriteLayer *layer);

/* What the branch snapshot carries (machine.cpp nm_snapshot_save/load) */
size_t ws_saved_size(void);
void ws_saved_get(void *dst);
void ws_saved_put(const void *src);

/* The game's native branches (DK_MODE_NATIVE, recomp/game/c) */
void ws_scene_begin(uint32_t header);   /* Scene_InitCommon: the scene header a scene starts from */
int  ws_act(void);                      /* the gameplay mode's margin per side (0: presentation only, 4:3) */
int  ws_place_left(void);               /* R20: px the wide view shows left / right of the 4:3 view, for */
int  ws_place_right(void);              /*   the screen-placed spawns (dk_ws.h ws_place_x) */
int  ws_area_left(void);                /* the same for the players' movement area and its edge tests */
int  ws_area_right(void);
int  ws_win_left(void);                 /* the same for R4's activation and kill windows */
int  ws_win_right(void);
void ws_note_place(int kind, int sx, int moved, uint32_t obj, uint32_t what);   /* R20's measurement */

#ifdef __cplusplus
}
#include <string>
std::string ws_json_extra(void);        /* the ws counters' diagnostics (first errors) */
#endif
