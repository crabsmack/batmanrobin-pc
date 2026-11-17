/* types/Pad.h - the joypad records (g_Pad1/g_Pad2 read on the V-int's full path, g_PadLive1/2
 * refreshed on every V-int; docs/NOTES.md "Game flow"). */
#pragma once
#include "dk_mem.h"

/* One pad's buttons: bit 7 Start, 6 A, 5 C, 4 B, 3 Right, 2 Left, 1 Down, 0 Up. */
typedef struct PadRec {
    u8 held;               /* +0: buttons down now */
    u8 prev;               /* +1: buttons down the read before */
    u8 pressed;            /* +2: newly pressed */
    u8 released;           /* +3: newly released */
} PadRec;
static_assert(sizeof(PadRec) == 4, "PadRec");
