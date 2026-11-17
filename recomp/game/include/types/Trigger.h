/* types/Trigger.h - the stage trigger tables (docs/NOTES.md "Triggers"). */
#pragma once
#include "dk_mem.h"

/* An entry of a trigger table (after the table's count word). */
typedef struct TriggerEntry {
    u16 size;              /* +$0: bytes to the next entry */
    u16 x_key;             /* +$2: fires when this x enters the screen (Trigger_RunX) */
    u16 y_key;             /* +$4: the same for the vertical stage (Trigger_RunY) */
    s16 x;                 /* +$6: d4 for the callback */
    s16 y;                 /* +$8: d5 for the callback */
    u32 callback;          /* +$A: code; its top byte a spawn action for the spawned object */
} TriggerEntry;
static_assert(offsetof(TriggerEntry, x_key) == 0x2, "TriggerEntry.x_key");
static_assert(offsetof(TriggerEntry, y_key) == 0x4, "TriggerEntry.y_key");
static_assert(offsetof(TriggerEntry, callback) == 0xA, "TriggerEntry.callback");

/* An element of g_TriggerList, the loaded table sorted by key: a 0 element first, the $FFFF
   sentinel last (Trigger_LoadX / Trigger_LoadY). */
typedef struct TriggerKey {
    u16 key;               /* +0: the entry's x key (y key on the vertical stage) */
    u16 offset;            /* +2: the entry's offset from g_TriggerTable */
} TriggerKey;
static_assert(sizeof(TriggerKey) == 4, "TriggerKey");
