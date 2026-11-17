/* types/Item.h - a weapon / power-up pickup: the a2-script object block (a6) as the Item natives
 * use it (docs/NOTES.md "a2 script objects"; Item_WeaponItemInit's header). A view of the object's
 * first $2E bytes: fields other modules name live in the general object struct when it exists. */
#pragma once
#include "dk_mem.h"

typedef struct ItemObj {
    u8  unk_00[0x1E];      /* +$00 */
    PTR(const s16) cycle;  /* +$1E: the value cycle (Item_WeaponItemNextValue steps it) */
    u8  unk_22[0x08];      /* +$22 */
    u16 value;             /* +$2A: the shown and collected value (0 = level down, 1-3 = weapon type + 1) */
    u16 next_value;        /* +$2C: the value prepared by the cycle */
} ItemObj;
static_assert(offsetof(ItemObj, cycle) == 0x1E, "ItemObj.cycle");
static_assert(offsetof(ItemObj, value) == 0x2A, "ItemObj.value");
static_assert(offsetof(ItemObj, next_value) == 0x2C, "ItemObj.next_value");
