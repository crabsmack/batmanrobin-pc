/* types/Hitbox.h - a collision box (docs/NOTES.md "Hit boxes", "Collision boxes"): the fields the C
 * uses so far. */
#pragma once
#include "dk_mem.h"

/* box flags (byte +$3C) */
enum {
    BOX_STRUCK = 1 << 0,   /* set by the hit callbacks when they accept a hit (g_StruckBox tests and clears it) */
};

typedef struct HitBox {
    s16 next;              /* +$00: the collision list's next link (0 = the tail sentinel) */
    s16 prev;              /* +$02: its prev link; 0 = on no list (Obj_SetBodyBoxes unlinks only then) */
    u16 serial;            /* +$04: the owner's serial ($56) */
    u16 kind;              /* +$06: the spec's kind word (which list the box goes on) */
    s16 spec_left;         /* +$08: the spec's rectangle around the owner: left, top, right, bottom */
    s16 spec_top;
    s16 spec_right;
    s16 spec_bottom;
    u16 id;               /* +$10: the owner id (owner $58: 1/2 players, 5, $A/$B enemies; Op2_SetBodyBoxId) */
    s16 left;              /* +$12: the rectangle in world pixels: left, top, right, bottom */
    s16 top;
    s16 right;
    s16 bottom;
    union {
        u16 struck_by;     /* +$1A: the owner id of the attacker whose box struck this one (Hitbox_Strike) */
        u16 hit_type;      /* ... then overwritten by the hit callbacks: 6 flinch, 7/8 knockback, 9 knockdown */
    };
    u8  unk_1C[0x02];
    s16 energy;            /* +$1E: the owner's energy (PlayerAtk_ApplyHit / Shot_DamageTarget subtract the damage) */
    u16 damage;            /* +$20: damage taken this frame (the hit callbacks add theirs) */
    s32 knockback;         /* +$22: x knockback of the hit (signed by the attacker's facing; PlayerAtk_ApplyHit) */
    u8  unk_26[0x08];
    s16 owner;             /* +$2E: handle of the object that owns the box (its +$58 = owner id) */
    u32 on_hit;            /* +$30: the box's hit callback (code; Native_NoOp for none) */
    u16 hit_damage;        /* +$34: the callback's arguments (the box spec's words): damage dealt */
    s16 hit_knockback;     /* +$36: knockback dealt (x 256 = 16.16) */
    u8  unk_38[0x04];
    u8  flags;             /* +$3C: BOX_STRUCK */
    u8  unk_3D;            /* cleared with +$3C (clr.w) by Obj_SetAttackBoxes / Obj_SetBodyBoxes */
} HitBox;
static_assert(offsetof(HitBox, prev) == 0x02, "HitBox.prev");
static_assert(offsetof(HitBox, serial) == 0x04, "HitBox.serial");
static_assert(offsetof(HitBox, spec_left) == 0x08, "HitBox.spec_left");
static_assert(offsetof(HitBox, id) == 0x10, "HitBox.id");
static_assert(offsetof(HitBox, left) == 0x12, "HitBox.left");
static_assert(offsetof(HitBox, bottom) == 0x18, "HitBox.bottom");
static_assert(offsetof(HitBox, struck_by) == 0x1A, "HitBox.struck_by");
static_assert(offsetof(HitBox, energy) == 0x1E, "HitBox.energy");
static_assert(offsetof(HitBox, damage) == 0x20, "HitBox.damage");
static_assert(offsetof(HitBox, knockback) == 0x22, "HitBox.knockback");
static_assert(offsetof(HitBox, owner) == 0x2E, "HitBox.owner");
static_assert(offsetof(HitBox, on_hit) == 0x30, "HitBox.on_hit");
static_assert(offsetof(HitBox, hit_damage) == 0x34, "HitBox.hit_damage");
static_assert(offsetof(HitBox, hit_knockback) == 0x36, "HitBox.hit_knockback");
static_assert(offsetof(HitBox, flags) == 0x3C, "HitBox.flags");
