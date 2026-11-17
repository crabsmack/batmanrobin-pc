/* types/Text.h - the sprite text's glyph table (docs/NOTES.md "Sprite text"). */
#pragma once
#include "dk_mem.h"

/* One character of Tbl_0141B2 (8 bytes per character from $20, indexed by char - $20). */
typedef struct TextGlyph {
    s16 width;             /* +0: x advance (Text_DrawSprites); the width Text_DrawSpritesVertical centres */
    s16 height;            /* +2: the y step down to the next letter (Text_DrawSpritesVertical) */
    s16 slot;              /* +4: byte offset into g_TextGlyphFrames (the frame held for it); < 0 = a blank */
    u16 frame;             /* +6: the letter's sprite frame (Spr_ChangeFrame's d1) */
} TextGlyph;
static_assert(sizeof(TextGlyph) == 8, "TextGlyph");
static_assert(offsetof(TextGlyph, slot) == 4, "TextGlyph.slot");
static_assert(offsetof(TextGlyph, frame) == 6, "TextGlyph.frame");
