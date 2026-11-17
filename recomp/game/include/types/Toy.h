/* types/Toy.h - stage 3's toy chains and toy rings (docs/NOTES.md "Stage 3 toy figures, chains and
 * rings"): the ring block at $E(a6) of a ring object, a chain's links (the chain block is in
 * types/ToyChain.h) and the accessors of the extra block. Views of
 * the fields their code uses; the rest stay unk_XX. */
#pragma once
#include "dk_mem.h"
#include "types/Hitbox.h"
#include "types/VM2.h"
#include "types/ToyChain.h"   /* ToyChainBlock */

enum {
    TOY_RING_MAX_FIGURES = 27,     /* figure frame handles that fit after +$24 */
};

/* one link of a chain: a pool block used as a collision box (ToyChain_AddLink) plus the figure
   drawn there */
typedef struct ToyChainLink {
    HitBox box;                    /* +$00: the collision box (on_hit = Native_NoOp $82C) */
    u32 hit;                       /* +$3E: the chain's link hit callback (run on a player box it touches) */
    u16 frame;                     /* +$42: the figure's sprite frame handle */
    s16 x;                         /* +$44: the link's position (world pixels) */
    s16 y;                         /* +$46 */
} ToyChainLink;
static_assert(offsetof(ToyChainLink, hit) == 0x3E, "ToyChainLink.hit");
static_assert(offsetof(ToyChainLink, frame) == 0x42, "ToyChainLink.frame");
static_assert(offsetof(ToyChainLink, x) == 0x44, "ToyChainLink.x");

/* the ring block: the ring object's extra pool block ($E(a6)), set up by ToyRing_Init */
typedef struct ToyRingBlock {
    s32 radius;                    /* +$00: the ring's radius, 16.16 */
    s32 last_radius_y;             /* +$04: the y radius ToyRing_Update drew last frame */
    s32 squash;                    /* +$08: squash, 16.16 (x radius r - squash/2, y radius r + squash) */
    s32 squash_speed;              /* +$0C */
    s32 stiffness;                 /* +$10: the spring's pull per frame (word << 8) */
    s32 spin;                      /* +$14: the ring's turn, 16.16 heading units */
    s32 step;                      /* +$18: the angle between two figures ($4000 / count, 16.16 >> 4) */
    u16 blink_pattern;             /* +$1C */
    u16 blink_mask;                /* +$1E: this frame's pattern, rotated once per figure drawn */
    s16 blink_timer;               /* +$20 */
    u16 blink_reload;              /* +$22 */
    u16 frame[TOY_RING_MAX_FIGURES];   /* +$24: the figures' sprite frame handles */
} ToyRingBlock;
static_assert(offsetof(ToyRingBlock, squash) == 0x08, "ToyRingBlock.squash");
static_assert(offsetof(ToyRingBlock, stiffness) == 0x10, "ToyRingBlock.stiffness");
static_assert(offsetof(ToyRingBlock, step) == 0x18, "ToyRingBlock.step");
static_assert(offsetof(ToyRingBlock, blink_pattern) == 0x1C, "ToyRingBlock.blink_pattern");
static_assert(offsetof(ToyRingBlock, blink_reload) == 0x22, "ToyRingBlock.blink_reload");
static_assert(offsetof(ToyRingBlock, frame) == 0x24, "ToyRingBlock.frame");
static_assert(sizeof(ToyRingBlock) <= 0x5A, "ToyRingBlock: one pool block");

/* a chain or ring object (a Vm2Obj) as their code sees it: +$E is the extra block */
typedef struct ToyObj {
    u8  unk_00[0x0E];
    s16 extra;                     /* +$0E: handle of its ToyChainBlock / ToyRingBlock */
} ToyObj;
static_assert(offsetof(ToyObj, extra) == 0x0E, "ToyObj.extra");

static inline int16_t toy_extra(const Vm2Obj *o) { return ((const ToyObj *)(const void *)o)->extra; }
static inline ToyChainBlock *toy_chain_block(const Vm2Obj *o) { return dk::handle<ToyChainBlock>(toy_extra(o)); }
static inline ToyRingBlock *toy_ring_block(const Vm2Obj *o) { return dk::handle<ToyRingBlock>(toy_extra(o)); }
