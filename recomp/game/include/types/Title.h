/* types/Title.h - the title screen's palette fade-in object (docs/NOTES.md "Front-end scenes"):
 * one Obj_NewClear block whose handler Title_PaletteFadeRun steps 7 colour-range channels. */
#pragma once
#include "dk_mem.h"

/* the ROM part of a channel (Tbl_04984C, after its delay and start level): how one colour range
   is lit */
typedef struct TitleFadeRange {
    u16 period;                    /* +0: frames between brightness steps, minus 1 */
    s16 reload_flag;               /* +2: the line's g_PalNReload flag (a RAM word address) */
    u16 count_minus1;              /* +4: colours in the range, minus 1 */
    s16 buffer_offset;             /* +6: the range's first colour in the palette buffer */
    s16 dest;                      /* +8: where it goes in g_PalSource (a RAM word address) */
} TitleFadeRange;
static_assert(sizeof(TitleFadeRange) == 10, "TitleFadeRange");

/* one entry of Tbl_04984C: 7 of them */
typedef struct TitleFadeEntry {
    u16 delay;                     /* +0: frames before the range starts lighting */
    s16 level;                     /* +2: its start brightness (-$10) */
    TitleFadeRange range;          /* +4 */
} TitleFadeEntry;
static_assert(sizeof(TitleFadeEntry) == 14, "TitleFadeEntry");

typedef struct TitleFadeChannel {  /* 10 bytes, from the object's +$E */
    u16 delay;                     /* +0: start delay, counted down to 0 */
    u16 period_count;              /* +2: frames to the next step (wraps below 0 = step now) */
    s16 level;                     /* +4: brightness, -$10 up to 0 in steps of 2 */
    PTR(const TitleFadeRange) range; /* +6 */
} TitleFadeChannel;
static_assert(sizeof(TitleFadeChannel) == 10, "TitleFadeChannel");

typedef struct TitleFadeObj {      /* a6 */
    u8  unk_00[0x0A];              /* +0 handler, +4/+6 list links, +8 cleared */
    PTR(const u16) palette;        /* +$A: the title palette (its colours) in the scene buffer */
    TitleFadeChannel ch[7];        /* +$E */
} TitleFadeObj;
static_assert(offsetof(TitleFadeObj, palette) == 0x0A, "TitleFadeObj.palette");
static_assert(offsetof(TitleFadeObj, ch) == 0x0E, "TitleFadeObj.ch");
