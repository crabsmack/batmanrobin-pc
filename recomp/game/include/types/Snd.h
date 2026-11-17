/* types/Snd.h - the sound driver's 68000-side records (docs/NOTES.md, "Sound driver (68000
 * side)"): the sound-effect request slots the V-int hands to the Z80, and the ROM table of
 * effect definitions Snd_PlaySfx fills them from. */
#pragma once
#include "dk_mem.h"

/* a ROM address the driver code computes (a table entry, a song or channel header), as a
   pointer */
template <class T> inline const T *snd_rom(uint32_t a)
{
    PTR(const T) p;
    p.v = a;
    return p;
}

/* one sound-effect request slot (g_SndSfxSlots, 6 x 6 bytes): Snd_PlaySfx fills it, the V-int's
   Snd_SendSfxBody copies the 6 bytes to the Z80's request area while `data` is nonzero, then
   clears `data`; an idle slot counts `hold` down and then drops its priority */
typedef struct SndSfxSlot {
    u8 priority;                   /* +0: a request below it does not take the slot */
    u8 hold;                       /* +1: frames the priority is kept after the hand-over */
    u32 data;                      /* +2: the effect's data (Snd_1E53A6 ...), 0 = nothing pending */
} SndSfxSlot;
static_assert(sizeof(SndSfxSlot) == 6, "SndSfxSlot");
static_assert(offsetof(SndSfxSlot, data) == 2, "SndSfxSlot.data");

enum { SND_SFX_SLOTS = 6 };

#define g_SndSfxSlot ((SndSfxSlot *)g_SndSfxSlots)

/* an effect's definition (Tbl_1E50D6, ROM, 8 bytes per effect, effect 1 first) */
typedef struct SndSfxDef {
    u8 slot_offset;                /* +0: byte offset of its slot in g_SndSfxSlots (6 * slot) */
    u8 priority;                   /* +1: compared with the slot's priority (cmp.b / bcs) */
    u8 keep_priority;              /* +2: the slot's priority while it plays */
    u8 hold;                       /* +3: the slot's hold frames */
    u32 data;                      /* +4: the effect's data pointer for the Z80 */
} SndSfxDef;
static_assert(sizeof(SndSfxDef) == 8, "SndSfxDef");
static_assert(offsetof(SndSfxDef, data) == 4, "SndSfxDef.data");

/* a music module's entry in Tbl_1E7EE4 (ROM, 8 bytes per module) */
typedef struct SndModule {
    PTR(const u8) songs;           /* +0: 3-byte big-endian song header pointers, one per song */
    PTR(const u8) header;          /* +4: the module header (bank pointers at +7 and +13) */
} SndModule;
static_assert(sizeof(SndModule) == 8, "SndModule");

/* a song's (song in its module, module offset) pair in the table at $1E6028 (ROM) */
typedef struct SndSongRef {
    u16 song;                      /* +0: the song's number inside its module */
    u16 module_offset;             /* +2: byte offset of its SndModule in Tbl_1E7EE4 (0/8/$10) */
} SndSongRef;
static_assert(sizeof(SndSongRef) == 4, "SndSongRef");
