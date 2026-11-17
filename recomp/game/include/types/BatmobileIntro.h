/* types/BatmobileIntro.h - the stage-opening Batmobile scene's object (Scr2_04A1A8, an a2 script
 * object): two frame timers of its own in the a6 block (docs/NOTES.md "Stage cards"). */
#pragma once
#include "dk_mem.h"

typedef struct BatmobileObj {      /* a6 */
    u8  unk_00[0x2A];
    u16 pal_timer;                 /* +$2A: 3..0, the speed-streak colours rotate when it wraps */
    u16 bump_timer;                /* +$2C: 5..0; bit 2 set (2 frames of 6) = the car's bump */
} BatmobileObj;
static_assert(offsetof(BatmobileObj, pal_timer) == 0x2A, "BatmobileObj.pal_timer");
static_assert(offsetof(BatmobileObj, bump_timer) == 0x2C, "BatmobileObj.bump_timer");
