/* types/SprList.h - the per-frame sprite display list (docs/NOTES.md "Sprite display list"):
 * Spr_ResetList empties it, Spr_AddSorted / AddBack / AddFront fill it, Spr_BuildSAT walks it.
 * Every link is a word handle ($FFFF9xxx-$FFFFAxxx RAM) of a node base: a node's sprite entry is at
 * +6, so a chained entry E is linked as E - 6. Key nodes are carved downwards from
 * g_SprNodeAlloc (18 bytes each, also for a 12-byte chained entry); the front list grows upwards
 * from g_SprFrontEnd. */
#pragma once
#include "dk_mem.h"

/* one sprite: its link to the next node base, position, attributes and frame-table entry */
typedef struct SprEntry {
    u16 link;                  /* +0: the next node base ($FFFF ends the list) */
    s16 x;                     /* +2 */
    s16 y;                     /* +4 */
    u16 attr;                  /* +6: palette / priority / flips */
    PTR(const void) frame;     /* +8: the 10-byte frame-table entry */
} SprEntry;
static_assert(sizeof(SprEntry) == 12, "SprEntry");

/* a depth key of the sorted list: its key, the next key node, its chain's last entry, its first
   entry */
typedef struct SprKeyNode {
    u16 key;                   /* +0: depth key (Spr_AddSorted's d7; descending) */
    u16 next;                  /* +2: the next key node */
    u16 last;                  /* +4: the last entry of this key's chain (its link word) */
    SprEntry entry;            /* +6 */
} SprKeyNode;
static_assert(offsetof(SprKeyNode, entry) == 6, "SprKeyNode.entry");
static_assert(sizeof(SprKeyNode) == 18, "SprKeyNode");

/* the head and tail sentinels at g_SprSortHead ($9DE0, $9DF2), the AddBack slot after them and
   the front list's start ($9E16); the key-node pool's top ($A3A4, carved downwards) */
static constexpr int SPR_TAIL_OFFSET = 0x12;     /* tail sentinel = head + $12 */
static constexpr int SPR_FRONT_OFFSET = 0x36;    /* the front list's first node base */
static constexpr uint16_t SPR_NODE_POOL_TOP = 0xA3A4;
static constexpr int SPR_NODES = 80;

/* a display-list node by its word handle */
static inline SprKeyNode *spr_node(uint16_t h) { return dk::handle<SprKeyNode>((int16_t)h); }
static inline SprEntry *spr_entry(uint16_t h) { return dk::handle<SprEntry>((int16_t)h); }
static inline u16 *spr_word(uint16_t h) { return dk::handle<u16>((int16_t)h); }

#if DK_MODE_NATIVE
/* Phase 6 (R10, E5): the native port's host side of the list and table (nodes past the 80-entry
   cap, the sprites past the VDP's 80): Spr_ResetList, Spr_Add*, Spr_BuildSAT, Spr_QueueSATDMA. */
#include "native/spr_host.h"
/* Phase 6 (R11, E3): the widescreen margin draws (Op2_DrawInScreenBox, the same list functions) */
#include "native/ws_draw.h"
#endif
