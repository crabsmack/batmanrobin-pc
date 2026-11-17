/* types/Story.h - the camera objects of the idle story scenes (Story_*, docs/NOTES.md "Front-end
 * scenes"): a2 script objects whose per-frame natives turn the object's position into the planes'
 * scroll. Views of the fields those natives use. */
#pragma once
#include "dk_mem.h"

typedef struct StoryCamObj {       /* a6 */
    u8  unk_00[0x2A];
    u16 counter;                   /* +$2A: the native's own frame counter (searchlight: the wobble
                                      phase, +2 a frame; monitors: the alarm tint's 8-frame toggle) */
    u8  unk_2C[0x12];
    s16 x;                         /* +$3E: world x, integer part (Vm2Obj.x) */
    u16 x_frac;
    s16 y;                         /* +$42: world y, integer part (Vm2Obj.y) */
    u16 y_frac;
    s32 drift;                     /* +$46: searchlight: plane A's drift, 16.16 (+$6000 a frame) */
    u8  unk_4A[0x04];
    s16 draw_x;                    /* +$4E: gate: where its sprites are drawn (screen x) */
    s16 draw_y;                    /* +$50: and y */
} StoryCamObj;
static_assert(offsetof(StoryCamObj, counter) == 0x2A, "StoryCamObj.counter");
static_assert(offsetof(StoryCamObj, x) == 0x3E, "StoryCamObj.x");
static_assert(offsetof(StoryCamObj, y) == 0x42, "StoryCamObj.y");
static_assert(offsetof(StoryCamObj, drift) == 0x46, "StoryCamObj.drift");
static_assert(offsetof(StoryCamObj, draw_x) == 0x4E, "StoryCamObj.draw_x");
static_assert(offsetof(StoryCamObj, draw_y) == 0x50, "StoryCamObj.draw_y");
