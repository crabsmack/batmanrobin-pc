/* types/Weapon.h - the weapon-shot art slots (g_ShotArtSlots: one per player, docs/NOTES.md
 * "Weapons and hits"). */
#pragma once
#include "dk_mem.h"

typedef struct ShotArtSlot {
    u16 frame;             /* +0: the shot frame whose art is held (Spr_ChangeFrame), 0 = none */
    u16 release_timer;     /* +2: frames until the previous frame is released (20 after a change) */
    u16 old_frame;         /* +4: the previous frame, released when the timer runs out */
} ShotArtSlot;
static_assert(sizeof(ShotArtSlot) == 6, "ShotArtSlot");
