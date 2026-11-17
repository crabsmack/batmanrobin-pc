/* types/Flight.h - the flying section's corridor work block (g_SceneVars in scene $33274;
 * docs/NOTES.md "Flying-section corridor"): Flight_UpdateCorridor's inputs, Flight_DrawCorridor's
 * work values and line budget, the middle wall's band table. */
#pragma once
#include "dk_mem.h"

typedef struct FlightCorridorVars { /* g_SceneVars in the corridor */
    s32 vpos;                       /* +$00: the vertical position, -(camera y * 1.25 + $E8), 24-bit */
    s32 phase;                      /* +$04: the floor/ceiling stripe phase (g_MapScrollX mod $A000) */
    s32 work_quarter;               /* +$08: phase / 4 (Flight_DrawCorridor: the floor spans' base) */
    s32 work_rest;                  /* +$0C: (phase - phase / 4) >> 8: divided by each span's height */
    s16 lines_left;                 /* +$10: lines of the 224 still to draw (Flight_DrawCorridor
                                       starts it at $E0; each span or band takes its share) */
    s32 band[32];                   /* +$12: the middle wall's band H-scroll offsets (16.16; the
                                       high word is drawn), advanced by Tbl_024880's rates */
} FlightCorridorVars;
static_assert(offsetof(FlightCorridorVars, phase) == 0x04, "FlightCorridorVars.phase");
static_assert(offsetof(FlightCorridorVars, lines_left) == 0x10, "FlightCorridorVars.lines_left");
static_assert(offsetof(FlightCorridorVars, band) == 0x12, "FlightCorridorVars.band");

#define g_FlightCorridor ((FlightCorridorVars *)g_SceneVars)

/* the airship hull part's scene variables (docs/NOTES.md "Airship hull objects"): after the
   corridor's bands, the path object whose position is the hull map's scroll offset */
typedef struct FlightAirshipVars {  /* g_SceneVars in the airship hull part */
    u8  unk_00[0x92];               /* +$00: the corridor's block (FlightCorridorVars) */
    s16 path_obj;                   /* +$92: handle of the path object (Flight_AirshipPathObjPark) */
    s16 map_x;                      /* +$94: map position of world (0, 0) = the path object's x/y */
    s16 map_y;                      /* +$96: (map = world + offset; Flight_AirshipSetMapOrigin) */
} FlightAirshipVars;
static_assert(offsetof(FlightAirshipVars, path_obj) == 0x92, "FlightAirshipVars.path_obj");
static_assert(offsetof(FlightAirshipVars, map_x) == 0x94, "FlightAirshipVars.map_x");
static_assert(offsetof(FlightAirshipVars, map_y) == 0x96, "FlightAirshipVars.map_y");

#define g_FlightAirship ((FlightAirshipVars *)g_SceneVars)

/* the flying section scene's own variables (Flight_RunScene and its parts, every part of the
   section; docs/NOTES.md "Flying-section scene and checkpoints"), after the corridor's block and
   the airship's map offset */
typedef struct FlightSceneVars {    /* g_SceneVars in the whole flying section */
    u8  unk_00[0x98];               /* +$00: FlightCorridorVars, then FlightAirshipVars' path/offset */
    u32 frame_fn;                   /* +$98: the part's frame callback (code), called by
                                       Flight_RunScene every frame (Flight_Start* set it) */
    s16 sky_far_x;                  /* +$9C: the sky's two interleaved cloud layers' H-scroll: */
    s16 sky_near_x;                 /* +$9E: -7 and -3 px a frame (Flight_DrawSky) */
    u16 wobble_phase;               /* +$A0: the cloud bank's wobble table phase, +2 a frame */
    u8  beam_flash;                 /* +$A2: set = the beam ship's death flash (only the disc) */
    u8  clouds_on;                  /* +$A3: set while the cloud bank is on screen
                                       (Flight_WaitCloudsPassed holds the loader script) */
} FlightSceneVars;
static_assert(offsetof(FlightSceneVars, frame_fn) == 0x98, "FlightSceneVars.frame_fn");
static_assert(offsetof(FlightSceneVars, sky_far_x) == 0x9C, "FlightSceneVars.sky_far_x");
static_assert(offsetof(FlightSceneVars, wobble_phase) == 0xA0, "FlightSceneVars.wobble_phase");
static_assert(offsetof(FlightSceneVars, beam_flash) == 0xA2, "FlightSceneVars.beam_flash");
static_assert(offsetof(FlightSceneVars, clouds_on) == 0xA3, "FlightSceneVars.clouds_on");

#define g_FlightScene ((FlightSceneVars *)g_SceneVars)

/* the cloud bank's wobble table (Flight_BuildWobbleTable): one sine period of 256 words, then the
   first 224 again, so any 224-line window reads without wrapping */
#define g_FlightWobble ((s16 *)(g_SceneWorkBuf + 0x400u))
enum { FLIGHT_WOBBLE_PERIOD = 0x100, FLIGHT_WOBBLE_LINES = 0xE0 };
