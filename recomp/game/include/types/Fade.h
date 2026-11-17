/* types/Fade.h - the palette fade's per-line records (docs/NOTES.md, the palette pipeline):
 * Fade_Step builds g_PalFaded from g_PalSource + g_FadeLevel + each line's tint offsets. */
#pragma once
#include "dk_mem.h"

/* one palette line's brightness offsets, signed, added to g_FadeLevel per channel ($F4C7 +
   4 * line: g_Pal0TintR .. g_Pal3Reload) */
typedef struct PalLineTint {
    s8 r;                          /* +0 */
    s8 g;                          /* +1 */
    s8 b;                          /* +2 */
    u8 reload;                     /* +3: g_PalNReload, the line was reloaded into g_PalSource */
} PalLineTint;
static_assert(sizeof(PalLineTint) == 4, "PalLineTint");

/* the four lines' tint records, from g_Pal0TintR */
#define g_PalLineTint ((PalLineTint *)&g_Pal0TintR)

/* per palette line, the brightness Fade_Step last built it with ($F4D7 + 4 * line) */
typedef struct PalLineCacheRec {
    s8 r;                          /* +0: g_FadeLevel + the line's red tint */
    s8 g;                          /* +1 */
    s8 b;                          /* +2 */
    u8 changed;                    /* +3: g_PalNChanged: $FF = rebuilt this V-int (upload it) */
} PalLineCacheRec;
static_assert(sizeof(PalLineCacheRec) == 4, "PalLineCacheRec");

#define g_PalLineCacheRec ((PalLineCacheRec *)g_PalLineCache)
