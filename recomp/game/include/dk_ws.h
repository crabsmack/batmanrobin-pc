/* dk_ws.h - the native runner's widescreen hooks the game's native branches call (phase 6, E3:
 * docs/phases/P6-enhance.md R4, R11, R12, R16, R20; recomp/platform/native/widescreen.{h,cpp}). Native
 * only (ruling R2): in the emulated-PC modes every hook is its 4:3 value, so the branches compile
 * to the original and verify keeps proving the C. */
#pragma once
#include <stdint.h>

#if DK_MODE_NATIVE
extern "C" {
void ws_scene_begin(uint32_t header);   /* Scene_InitCommon: the header the scene starts from */
int  ws_act(void);                      /* the gameplay mode's margin (R4): px per side objects activate past 320 */
/* The gameplay mode's wide view as the player sees it (R20): px it shows left of the 4:3 view's
   x 0 and right of its x 320 (the render camera at a streamed map's ends, the bars where a picture
   drawn whole ends; 40 / 40 mid-level); 0 / 0 in the presentation mode and with widescreen off.
   PLACE: screen-placed spawns; AREA: the players' movement area and the edge tests kept with it. */
int  ws_place_left(void);
int  ws_place_right(void);
int  ws_area_left(void);
int  ws_area_right(void);
/* WIN: the windows that create and remove objects (activation, the kill windows: R4's WS_ACT
   sites), per side, so they follow the view where the render camera makes one margin 80 and the
   other 0 (a streamed map's ends): a spawn placed past an 80-px margin is not inside a 40-px kill
   window's edge. 40 / 40 mid-level, as WS_ACT. */
int  ws_win_left(void);
int  ws_win_right(void);
/* a measurement (BR_WS_PLAY_LOG, widescreen.cpp): an object placed relative to the screen at
   screen x sx, moved by `moved` px for the wide view; what = its spawner / script; obj its a6 */
void ws_note_place(int kind, int sx, int moved, uint32_t obj, uint32_t what);
}
#define WS_SCENE_BEGIN(h)   ws_scene_begin(h)
#define WS_ACT()            ((uint16_t)ws_act())
#define WS_PLACE_L()        ((uint16_t)ws_place_left())
#define WS_PLACE_R()        ((uint16_t)ws_place_right())
#define WS_AREA_L()         ((uint16_t)ws_area_left())
#define WS_AREA_R()         ((uint16_t)ws_area_right())
#define WS_WIN_L()          ((uint16_t)ws_win_left())
#define WS_WIN_R()          ((uint16_t)ws_win_right())
#define WS_NOTE_PLACE(kind, sx, moved, obj, what) ws_note_place((kind), (sx), (moved), (obj), (what))
#else
#define WS_SCENE_BEGIN(h)   ((void)0)
#define WS_ACT()            ((uint16_t)0)
#define WS_PLACE_L()        ((uint16_t)0)
#define WS_PLACE_R()        ((uint16_t)0)
#define WS_AREA_L()         ((uint16_t)0)
#define WS_AREA_R()         ((uint16_t)0)
#define WS_WIN_L()          ((uint16_t)0)
#define WS_WIN_R()          ((uint16_t)0)
#define WS_NOTE_PLACE(kind, sx, moved, obj, what) ((void)0)
#endif

/* the kinds ws_note_place counts (widescreen.cpp's log names them) */
enum { WS_PLACE_WAVE = 1, WS_PLACE_CAMERA_OFS = 2, WS_PLACE_ON_SCREEN = 3, WS_PLACE_POLICE_CAR = 4,
       WS_PLACE_WAVE_WORLD = 5 /* a world-placed wave entry: logged only (where it lands on screen) */,
       WS_PLACE_WAVE_FIXED = 6 /* a world entry under a camera locked at x 0: screen-placed */ };

/* R20: a screen x the game places an object at, for the wide view's edge: right of the 4:3 view
 * (x >= 320) moved out by the px the wide view shows right of it, left of it (x < 0) likewise, so
 * the object starts as far past the wide view's edge as the original's start was past 4:3's;
 * inside the 4:3 view unchanged. The identity in the PC modes and outside the gameplay mode. */
static inline uint16_t ws_place_x(uint16_t x)
{
    if ((int16_t)x >= 0x140)
        return (uint16_t)(x + WS_PLACE_R());
    if ((int16_t)x < 0)
        return (uint16_t)(x - WS_PLACE_L());
    return x;
}
