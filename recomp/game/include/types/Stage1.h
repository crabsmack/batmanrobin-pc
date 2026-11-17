/* types/Stage1.h - stage 1's set pieces and bosses (docs/NOTES.md "Stage 1", "Bosses"): their a6
 * objects are a2-script objects (types/VM2.h, Vm2Obj) whose spare fields their own natives give a
 * meaning; these are views with those fields named, the rest unk_XX. */
#pragma once
#include "dk_mem.h"

/* g_SceneVars ($F4F4) while the stage 1 street runs (scene $1FB3A): the per-scene scratch as its
   code uses it (NOTES "Stage 1 street"; other scenes give these bytes other meanings) */
typedef struct Stage1StreetVars {
    u32 fire_frame;                 /* +$00 ($F4F4): the fire's next frame entry (Tbl_015A7E, 20 bytes each) */
    u16 fire_frame_x;               /* +$04 ($F4F8): the drawn fire frame's x offset (Stage1_SwayFireLines) */
    u8  sign_wanted[4];             /* +$06 ($F4FA): the 4 sign slots' wanted panels: 0 intact, 8 blasted */
    u8  sign_drawn[4];              /* +$0A ($F4FE): the panels Stage1_VIntFacade last drew */
    u32 map_buffer;                 /* +$0E ($F502): the unpacked facade map the panels are copied from */
    u32 facade_x;                   /* +$12 ($F506): facade x, 16.16 (g_ViewX * 35/32) */
} Stage1StreetVars;
static_assert(offsetof(Stage1StreetVars, fire_frame_x) == 0x04, "Stage1StreetVars.fire_frame_x");
static_assert(offsetof(Stage1StreetVars, sign_wanted) == 0x06, "Stage1StreetVars.sign_wanted");
static_assert(offsetof(Stage1StreetVars, sign_drawn) == 0x0A, "Stage1StreetVars.sign_drawn");
static_assert(offsetof(Stage1StreetVars, map_buffer) == 0x0E, "Stage1StreetVars.map_buffer");
static_assert(offsetof(Stage1StreetVars, facade_x) == 0x12, "Stage1StreetVars.facade_x");

/* a sign slot's record (Tbl_015CF4, 16 bytes a slot): per panel (intact, blasted) the VDP write
   command of the panel's top-left cell in plane B and its offset in the facade map */
typedef struct Stage1SignSlot {
    struct {
        u32 vdp_command;
        u32 map_offset;
    } panel[2];
} Stage1SignSlot;
static_assert(sizeof(Stage1SignSlot) == 0x10, "Stage1SignSlot");

/* the Joker balloon boss's scroll-split V-int object (a V-int list block, Boss_JokerBalloon_StartSplit) */
typedef struct BalloonSplitObj {
    u32 handler;                    /* +$00: the V-int handler (Boss_JokerBalloon_VIntSplit) */
    u8  unk_04[0x14];
    s16 x;                          /* +$18: the balloon's world x (Boss_JokerBalloon_SyncSplit) */
    s16 y;                          /* +$1A: its world y; 0 = no split (Boss_JokerBalloon_HideSplit) */
} BalloonSplitObj;
static_assert(offsetof(BalloonSplitObj, x) == 0x18, "BalloonSplitObj.x");
static_assert(offsetof(BalloonSplitObj, y) == 0x1A, "BalloonSplitObj.y");

typedef struct BalloonObj {         /* a6: the Joker balloon boss (Scr2_01C4E6) */
    u8  unk_00[0x1E];
    s16 split;                      /* +$1E: handle of its BalloonSplitObj */
    u8  unk_20[0x1E];
    s16 x;                          /* +$3E: world x, integer part */
    u16 x_frac;
    s16 y;                          /* +$42: world y, integer part */
} BalloonObj;
static_assert(offsetof(BalloonObj, split) == 0x1E, "BalloonObj.split");
static_assert(offsetof(BalloonObj, x) == 0x3E, "BalloonObj.x");
static_assert(offsetof(BalloonObj, y) == 0x42, "BalloonObj.y");

typedef struct HarleyObj {          /* a6: the Harley mid-boss cart (Boss_Harley_*, Scr2_018066) */
    u8  unk_00[0x0E];
    s16 part_sprite;                /* +$0E: handle of its second sprite block (Harley on the pole) */
    s16 pole;                       /* +$10: handle of the telescoping pole's block (HarleyPole) */
    u8  unk_12[0x2C];
    s16 x;                          /* +$3E: world x, integer part */
    u16 x_frac;
    s16 y;                          /* +$42: world y, integer part */
    u16 y_frac;
    u8  unk_46[0x08];
    s16 drawn_x;                    /* +$4E: the world position its sprite is drawn at */
    s16 drawn_y;                    /* +$50 */
} HarleyObj;
static_assert(offsetof(HarleyObj, part_sprite) == 0x0E, "HarleyObj.part_sprite");
static_assert(offsetof(HarleyObj, pole) == 0x10, "HarleyObj.pole");

/* the Harley pole's block (HarleyObj.pole; shared by the cart and the pole segments): 16.16 */
typedef struct HarleyPole {
    s32 target;                     /* +$00: the height it moves toward */
    s32 height;                     /* +$04: its current height (px above the cart) */
    s32 vel;                        /* +$08 */
    s32 accel;                      /* +$0C: toward the target (Boss_Harley_SetPoleTarget) */
} HarleyPole;
static_assert(sizeof(HarleyPole) == 0x10, "HarleyPole");
static_assert(offsetof(HarleyObj, x) == 0x3E, "HarleyObj.x");
static_assert(offsetof(HarleyObj, y) == 0x42, "HarleyObj.y");
static_assert(offsetof(HarleyObj, drawn_x) == 0x4E, "HarleyObj.drawn_x");
static_assert(offsetof(HarleyObj, drawn_y) == 0x50, "HarleyObj.drawn_y");

typedef struct ClawObj {            /* a6: Harley's crane claw (Boss_Claw_*, Scr2_01E778; 8 pool blocks) */
    u8  unk_00[0x0E];
    s16 chain;                      /* +$0E: handle of the chain's sprite slots (12 balls) */
    s16 finger[3];                  /* +$10/$12/$14: handles of the three fingers' point lists */
    u8  unk_16[0x08];
    s32 swing_angle;                /* +$1E: pendulum angle, 16.16 */
    s32 swing_accel;                /* +$22: restoring acceleration (16.16 per frame) */
    s32 swing_vel;                  /* +$26: angular velocity (16.16 per frame) */
    u16 sort;                       /* +$2A: the balls' sort depth: $4000 in front, $1000 behind */
    u16 priority;                   /* +$2C: attribute bits ORed in: $8000 (priority) in front, 0 behind */
    u16 hittable;                   /* +$2E: 1 while the chain head's depth is $700-$800 (the players' plane) */
    u16 hit_pause;                  /* +$30: frames the claw holds still after a hit (Boss_Claw_HitCallback) */
    u8  unk_32[0x0C];
    s16 x;                          /* +$3E: world x, integer part */
    u16 x_frac;
    s16 y;                          /* +$42: world y, integer part */
    u16 y_frac;
    u8  unk_46[0x08];
    s16 drawn_x;                    /* +$4E: the chain head's projected world position */
    s16 drawn_y;                    /* +$50 */
} ClawObj;
static_assert(offsetof(ClawObj, chain) == 0x0E, "ClawObj.chain");
static_assert(offsetof(ClawObj, finger) == 0x10, "ClawObj.finger");
static_assert(offsetof(ClawObj, swing_angle) == 0x1E, "ClawObj.swing_angle");
static_assert(offsetof(ClawObj, swing_accel) == 0x22, "ClawObj.swing_accel");
static_assert(offsetof(ClawObj, swing_vel) == 0x26, "ClawObj.swing_vel");
static_assert(offsetof(ClawObj, sort) == 0x2A, "ClawObj.sort");
static_assert(offsetof(ClawObj, hittable) == 0x2E, "ClawObj.hittable");
static_assert(offsetof(ClawObj, hit_pause) == 0x30, "ClawObj.hit_pause");
static_assert(offsetof(ClawObj, x) == 0x3E, "ClawObj.x");
static_assert(offsetof(ClawObj, drawn_x) == 0x4E, "ClawObj.drawn_x");

/* one entry of a balloon launcher's height table (Tbl_020354 / Tbl_02036E): the two screen-y
   heights a balloon bomb bobs between; the table ends with a negative word */
typedef struct BalloonHeights {
    s16 height[2];
} BalloonHeights;
static_assert(sizeof(BalloonHeights) == 4, "BalloonHeights");

typedef struct BalloonLauncherObj { /* a6: the parade truck's unseen balloon-bomb emitter (Scr2_01FFB8 / Scr2_020094) */
    u8  unk_00[0x1E];
    PTR(const BalloonHeights) heights;   /* +$1E: the next pair to hand out (Stage1_LaunchBalloonA/B) */
    u8  unk_22[0x1C];
    s16 x;                          /* +$3E: world x, integer part */
    u16 x_frac;
    s16 y;                          /* +$42: world y, integer part */
} BalloonLauncherObj;
static_assert(offsetof(BalloonLauncherObj, heights) == 0x1E, "BalloonLauncherObj.heights");
static_assert(offsetof(BalloonLauncherObj, x) == 0x3E, "BalloonLauncherObj.x");
static_assert(offsetof(BalloonLauncherObj, y) == 0x42, "BalloonLauncherObj.y");

typedef struct BalloonBombObj {     /* a6: a balloon bomb (Scr2_0203E0) */
    u8  unk_00[0x2A];
    s16 height[2];                  /* +$2A: the screen y it cruises to, +$2C: the other one (swapped on arrival) */
    u8  unk_2E[0x14];
    s16 y;                          /* +$42: world y, integer part */
} BalloonBombObj;
static_assert(offsetof(BalloonBombObj, height) == 0x2A, "BalloonBombObj.height");
static_assert(offsetof(BalloonBombObj, y) == 0x42, "BalloonBombObj.y");

/* the debris burst of a blasted bank sign (Stage1_SpawnSignDebris): a 7-block run-list object whose
   6 extra blocks hold 3 pieces each */
typedef struct SignDebrisObj {
    u32 handler;                    /* +$00: Stage1_SignDebrisRun */
    u8  unk_04[0x04];
    s16 block[6];                   /* +$08: handles of the 6 piece blocks */
    u8  unk_14[0x04];
    u16 frame;                      /* +$18: the sprite frame all pieces share (Spr_ChangeFrame's slot) */
    s16 frames_left;                /* +$1A: life, from $46 (70 frames) */
} SignDebrisObj;
static_assert(offsetof(SignDebrisObj, block) == 0x08, "SignDebrisObj.block");
static_assert(offsetof(SignDebrisObj, frame) == 0x18, "SignDebrisObj.frame");
static_assert(offsetof(SignDebrisObj, frames_left) == 0x1A, "SignDebrisObj.frames_left");

typedef struct SignDebrisPiece {    /* one piece of the burst ($18 bytes, 3 to a block) */
    s32 vx;                         /* +$00: x speed, 16.16 */
    s16 x;                          /* +$04: world x, 16.16 */
    u16 x_frac;
    s32 vy;                         /* +$08: y speed, 16.16 (gravity $7000 per frame) */
    s16 y;                          /* +$0C: world y, 16.16 */
    u16 y_frac;
    u8  spin_rate;                  /* +$10: added to spin_acc each frame ($80-$BF) */
    u8  spin_acc;                   /* +$11: its carry steps the tumble */
    u16 tumble;                     /* +$12: the tumble frame offset 0-$32, step $A */
    u16 frame;                      /* +$14: the first tumble frame */
    u16 attr;                       /* +$16: the sprite attribute word */
} SignDebrisPiece;
static_assert(sizeof(SignDebrisPiece) == 0x18, "SignDebrisPiece");
static_assert(offsetof(SignDebrisPiece, spin_rate) == 0x10, "SignDebrisPiece.spin_rate");
static_assert(offsetof(SignDebrisPiece, attr) == 0x16, "SignDebrisPiece.attr");

/* a Tbl_02095E record: how one debris piece starts */
typedef struct SignDebrisInit {
    u16 frame;                      /* the first tumble frame */
    s16 dx, dy;                     /* offset from the burst centre */
    s16 vx, vy;                     /* launch speeds (x 256 into 16.16), plus random spread */
} SignDebrisInit;
static_assert(sizeof(SignDebrisInit) == 10, "SignDebrisInit");

/* the bank bombing's rubble rain (Stage1_StartRubbleRain): a one-block run-list object */
typedef struct RubbleRainObj {
    u32 handler;                    /* +$00: Stage1_RubbleRainRun */
    u8  unk_04[0x14];
    s16 frames_left;                /* +$18: from $578; it frees itself when this goes negative */
    s16 delay;                      /* +$1A: frames to the next piece (every 7th frame) */
    PTR(const u32) cursor;          /* +$1C: the next entry of Tbl_020C26 (piece records; 0 ends the list) */
} RubbleRainObj;
static_assert(offsetof(RubbleRainObj, frames_left) == 0x18, "RubbleRainObj.frames_left");
static_assert(offsetof(RubbleRainObj, cursor) == 0x1C, "RubbleRainObj.cursor");

typedef struct RubblePieceObj {     /* one falling piece of rubble (Stage1_RubblePieceRun) */
    u32 handler;                    /* +$00: Stage1_RubblePieceRun */
    u8  unk_04[0x1C];
    s16 x;                          /* +$20: world x, 16.16 */
    u16 x_frac;
    s16 y;                          /* +$24: world y, 16.16 */
    u16 y_frac;
    s32 vx;                         /* +$28: x speed, 16.16 */
    s32 ax;                         /* +$2C: x acceleration */
    s32 vy;                         /* +$30: y speed, 16.16 */
    s32 gravity;                    /* +$34 */
    s16 fall;                       /* +$38: frames of free fall left; then the terrain routine moves it */
    PTR(const s16) anim;            /* +$3A: its animation: rate, period, then the frames */
    u16 frame;                      /* +$3E: the sprite frame shown (Spr_ChangeFrame's slot) */
    u16 anim_phase;                 /* +$40: 8.8, the high byte indexes the frames */
} RubblePieceObj;
static_assert(offsetof(RubblePieceObj, x) == 0x20, "RubblePieceObj.x");
static_assert(offsetof(RubblePieceObj, vx) == 0x28, "RubblePieceObj.vx");
static_assert(offsetof(RubblePieceObj, gravity) == 0x34, "RubblePieceObj.gravity");
static_assert(offsetof(RubblePieceObj, fall) == 0x38, "RubblePieceObj.fall");
static_assert(offsetof(RubblePieceObj, anim) == 0x3A, "RubblePieceObj.anim");
static_assert(offsetof(RubblePieceObj, frame) == 0x3E, "RubblePieceObj.frame");
static_assert(offsetof(RubblePieceObj, anim_phase) == 0x40, "RubblePieceObj.anim_phase");

typedef struct BankBombingObj {     /* a6: the bank bombing director (Scr2_020CF6) */
    u8  unk_00[0x38];
    u32 next_wave;                  /* +$38: the wave script to start afterwards (the trigger entry's extra long) */
} BankBombingObj;
static_assert(offsetof(BankBombingObj, next_wave) == 0x38, "BankBombingObj.next_wave");

typedef struct FireFlickerObj {     /* a6: the fire's palette flicker (Stage1_StartFireFlicker) */
    u32 handler;                    /* +$00: Stage1_FireFlickerRun */
    u8  unk_04[0x14];
    s16 delay;                      /* +$18: frames to the next swap (random 0-7) */
    u16 toggle;                     /* +$1A: 0 / $FFFF: which palette was loaded last */
} FireFlickerObj;
static_assert(offsetof(FireFlickerObj, delay) == 0x18, "FireFlickerObj.delay");
static_assert(offsetof(FireFlickerObj, toggle) == 0x1A, "FireFlickerObj.toggle");
