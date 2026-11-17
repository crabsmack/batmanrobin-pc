/* types/ToyChain.h - stage 3's toy-figure chains (docs/NOTES.md "Stage 3 toy figures, chains and
 * rings"): the chain object made by ToyChain_SpawnInline and its chain block. Views of the fields
 * the chain natives use; the rest stay unk_XX. */
#pragma once
#include "dk_mem.h"

enum {
    TOY_CHAIN_MAX_LINKS = 19,      /* link handles that fit in the block after +$34 (a pool block is $5A) */
};

/* the chain block (a pool block, handle at $E of the chain object; ToyChain_SpawnInline fills it
   from the maker's table, ToyChain_Grow / ToyChain_PlaceAndDrawLinks run it) */
typedef struct ToyChainBlock {
    s32 sway_base;         /* +$00: added to the sway value (the chain's x, a6 +$3E) before the
                              per-link bend (its high word from the table, the low word 0) */
    s32 heading;           /* +$04: base heading, 16.16 (table word << 16 | $8000) */
    s16 sway_scale;        /* +$08: heading += (sway >> 8) * this */
    u8  unk_0A[0x02];
    s32 extension;         /* +$0C: growth of the newest link, 16.16 (a link is added at 0, then -20) */
    s32 grow_speed;        /* +$10: added to the extension per frame (table word << 8; 0 = one link a frame) */
    s32 regrow_speed;      /* +$14: the speed after the first links (regrowing links lost to hits) */
    u16 blink_pattern;     /* +$18: the charged blink's bit pattern (rotated per blink step) */
    u16 blink_mask;        /* +$1A: this frame's pattern, rotated once per link drawn (bit out = outlined) */
    s16 blink_timer;       /* +$1C: frames to the next blink step */
    u16 blink_reload;      /* +$1E */
    u16 link_energy;       /* +$20: energy of one link (the chain object's +$52 is reset to it) */
    u16 link_damage;       /* +$22: the links' hit callback arguments: damage */
    u16 link_knockback;    /* +$24: and knockback */
    u32 link_hit;          /* +$26: the links' hit callback (code) */
    u16 grow_left;         /* +$2A: links still to grow (the maker's link count at the spawn) */
    u16 links_max;         /* +$2C: the most links the chain regrows to */
    u16 frame_turn;        /* +$2E: added to each link's heading to pick its drawn rotation
                              (ToyChain_PlaceAndDrawLinks; $200 = half a turn) */
    s16 tip_x;             /* +$30: the last link's position as the links were last placed */
    s16 tip_y;             /* +$32 */
    s16 link[TOY_CHAIN_MAX_LINKS]; /* +$34: the links' block handles (ToyChainLink), base first */
} ToyChainBlock;
static_assert(offsetof(ToyChainBlock, heading) == 0x04, "ToyChainBlock.heading");
static_assert(offsetof(ToyChainBlock, sway_scale) == 0x08, "ToyChainBlock.sway_scale");
static_assert(offsetof(ToyChainBlock, extension) == 0x0C, "ToyChainBlock.extension");
static_assert(offsetof(ToyChainBlock, regrow_speed) == 0x14, "ToyChainBlock.regrow_speed");
static_assert(offsetof(ToyChainBlock, blink_pattern) == 0x18, "ToyChainBlock.blink_pattern");
static_assert(offsetof(ToyChainBlock, link_energy) == 0x20, "ToyChainBlock.link_energy");
static_assert(offsetof(ToyChainBlock, link_hit) == 0x26, "ToyChainBlock.link_hit");
static_assert(offsetof(ToyChainBlock, grow_left) == 0x2A, "ToyChainBlock.grow_left");
static_assert(offsetof(ToyChainBlock, frame_turn) == 0x2E, "ToyChainBlock.frame_turn");
static_assert(offsetof(ToyChainBlock, tip_x) == 0x30, "ToyChainBlock.tip_x");
static_assert(offsetof(ToyChainBlock, tip_y) == 0x32, "ToyChainBlock.tip_y");
static_assert(offsetof(ToyChainBlock, link) == 0x34, "ToyChainBlock.link");
static_assert(sizeof(ToyChainBlock) <= 0x5A, "ToyChainBlock: one pool block");

/* the chain object (a6, a Vm2Obj): the fields the chain gives a meaning of its own */
typedef struct ToyChainObj {
    u8  unk_00[0x0E];
    s16 chain_block;       /* +$0E: handle of the ToyChainBlock */
    u8  unk_10[0x2E];
    s32 sway;              /* +$3E: Vm2Obj.x read as 16.16: the sway value the a3 motion ops cruise
                              toward the player (not a position) */
    u8  unk_42[0x0C];
    s16 base_x;            /* +$4E: the drawn base (Vm2Obj.draw_x / draw_y): the links hang from it */
    s16 base_y;            /* +$50 */
} ToyChainObj;
static_assert(offsetof(ToyChainObj, chain_block) == 0x0E, "ToyChainObj.chain_block");
static_assert(offsetof(ToyChainObj, sway) == 0x3E, "ToyChainObj.sway");
static_assert(offsetof(ToyChainObj, base_x) == 0x4E, "ToyChainObj.base_x");
static_assert(offsetof(ToyChainObj, base_y) == 0x50, "ToyChainObj.base_y");

/* chain script variable 0 (Vm2Block.var[0]) */
enum {
    TOYCHAIN_LOST_LINK = 1 << 4,   /* a link was lost: ToyChain_SteerAtPlayer speeds the sway up once */
};
