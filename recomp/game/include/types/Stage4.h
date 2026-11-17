/* types/Stage4.h - stage 4's scene variables (g_SceneVars, docs/NOTES.md "Stage 4 sections and scroll"): what the lab
 * and the ring tunnel's per-frame scroll code keeps between frames. */
#pragma once
#include "dk_mem.h"

/* g_SceneVars in the lab (section $4300, Stage4_LabScroll and its line routines) */
typedef struct LabScrollVars {
    u32 floor_x;           /* +$00: g_ViewX * 35/32 (16.16); plane B reads its integer part */
    s32 offset;            /* +$04: -(the x within the current floor segment) << 8 */
    u16 segment;           /* +$08: floor segment number ($5700 >> 8 = 87 px each); the floor
                              lines mask it to its parity in place (andi.w #1) */
    s32 upper;             /* +$0A: the upper wall's perspective base (16.16) */
    u16 wobble;            /* +$0E: plane B's sine phase (+$A a frame) */
} LabScrollVars;
static_assert(offsetof(LabScrollVars, offset) == 0x04, "LabScrollVars.offset");
static_assert(offsetof(LabScrollVars, segment) == 0x08, "LabScrollVars.segment");
static_assert(offsetof(LabScrollVars, upper) == 0x0A, "LabScrollVars.upper");
static_assert(offsetof(LabScrollVars, wobble) == 0x0E, "LabScrollVars.wobble");

/* g_SceneVars in the ring tunnel (section $4400, Stage4_TubeScroll) */
typedef struct TubeScrollVars {
    s32 want_speed;        /* +$00: the speed the tunnel eases toward (16.16, negative = forward) */
    s32 speed;             /* +$04: the current speed, clamped -12..-2 px */
    s32 pos;               /* +$08: position within one 64-px ring, -64..0 (16.16) */
} TubeScrollVars;
static_assert(offsetof(TubeScrollVars, speed) == 0x04, "TubeScrollVars.speed");
static_assert(offsetof(TubeScrollVars, pos) == 0x08, "TubeScrollVars.pos");

/* a stage 4 enemy or boss object (a6, a Vm2Obj): the fields its natives use */
typedef struct S4Obj {
    u8  unk_00[0x1E];
    s16 cone;              /* +$1E: cone owners: handle of its g_ConeSlots slot (ConeSlot), 0 = none */
    u8  unk_20[0x0A];
    u16 sprite_frame;      /* +$2A: gas orbs: the puff sprite slot StepJet changes each puff to */
    u8  unk_2C[0x12];
    s16 x;                 /* +$3E: world x, integer part (Vm2Obj.x) */
    u16 x_frac;
    s16 y;                 /* +$42: world y, integer part (Vm2Obj.y) */
    u16 y_frac;
    u8  unk_46[0x08];
    s16 draw_x;            /* +$4E: where Op2_DrawSpr*AtDrawn draws it (world x) */
    s16 draw_y;            /* +$50: and y */
} S4Obj;
static_assert(offsetof(S4Obj, cone) == 0x1E, "S4Obj.cone");
static_assert(offsetof(S4Obj, sprite_frame) == 0x2A, "S4Obj.sprite_frame");
static_assert(offsetof(S4Obj, x) == 0x3E, "S4Obj.x");
static_assert(offsetof(S4Obj, y) == 0x42, "S4Obj.y");
static_assert(offsetof(S4Obj, draw_x) == 0x4E, "S4Obj.draw_x");
static_assert(offsetof(S4Obj, draw_y) == 0x50, "S4Obj.draw_y");

/* Mr. Freeze's hover-platform block ($12(a6), a3 of Boss_Freeze_DropStep) */
typedef struct FreezePlatform {
    u8  unk_00[0x0C];
    s32 drop;              /* +$0C: how far he has dropped below the platform (16.16, clamped at $50 px) */
} FreezePlatform;
static_assert(offsetof(FreezePlatform, drop) == 0x0C, "FreezePlatform.drop");

/* cone slot modes (ConeSlot.mode, set by Stage4_ConeSetInline) */
enum {
    CONE_SPIN = 0,         /* spins, a random colour each frame */
    CONE_SWEEP_ONCE = 1,   /* one sweep turn, following the owner's heading */
    CONE_SWEEP = 2,        /* sweeps by the owner's turn rate */
    CONE_CLOSE = 3,        /* closing: the slot frees itself at length 0 */
};

/* a translucent cone of the hall and the Freeze fight: one of three g_ConeSlots ($1E bytes each),
   owned by an object (S4Obj.cone), drawn by Stage4_DrawCones as a span triangle */
typedef struct ConeSlot {
    u8  in_use;            /* +$00: nonzero = owned */
    u8  mode;              /* +$01: CONE_SPIN .. CONE_CLOSE */
    u16 colour;            /* +$02: its colour set, a byte offset into Tbl_047508 (0/8/$10) */
    s16 apex_x;            /* +$04: the apex, screen x / y (the owner's position) */
    s16 apex_y;
    u16 spin_b;            /* +$08..+$0C: the first angle set (Rot3D angles b, a, c), which turns the
                              far edge in place */
    u16 spin_a;
    u16 spin_c;
    u16 view_b;            /* +$0E..+$12: the second set (b, a, c), the projection's */
    u16 view_a;
    u16 view_c;
    s16 depth;             /* +$14: the projection's z translation, integer part ($90) */
    s16 target_width;      /* +$16: what width / length ease toward (Stage4_ConeUpdate) */
    s16 target_length;
    s16 width;             /* +$1A: the far edge's current width / length */
    s16 length;
} ConeSlot;
static_assert(offsetof(ConeSlot, colour) == 0x02, "ConeSlot.colour");
static_assert(offsetof(ConeSlot, apex_x) == 0x04, "ConeSlot.apex_x");
static_assert(offsetof(ConeSlot, spin_b) == 0x08, "ConeSlot.spin_b");
static_assert(offsetof(ConeSlot, view_b) == 0x0E, "ConeSlot.view_b");
static_assert(offsetof(ConeSlot, depth) == 0x14, "ConeSlot.depth");
static_assert(offsetof(ConeSlot, target_width) == 0x16, "ConeSlot.target_width");
static_assert(offsetof(ConeSlot, width) == 0x1A, "ConeSlot.width");
static_assert(sizeof(ConeSlot) == 0x1E, "ConeSlot");

/* g_SceneVars in the hall (section $4000, Stage4_HallFloorRipple): the floor ripple's phases */
typedef struct HallScrollVars {
    u16 drift;             /* +$00: +1 a frame, added to the second phase */
    u8  unk_02[0x02];
    u16 phase_a;           /* +$04: +1 a frame */
    u16 phase_b;           /* +$06: +2 a frame */
} HallScrollVars;
static_assert(offsetof(HallScrollVars, phase_a) == 0x04, "HallScrollVars.phase_a");
static_assert(offsetof(HallScrollVars, phase_b) == 0x06, "HallScrollVars.phase_b");

/* one puff of a gas orb's jet: 5 of them in a pool block (handle in the orb's +$12/$14/$16),
   moved out along the jet by Enemy_GasOrb_StepJet */
typedef struct GasPuff {
    s32 radius;            /* +$00: distance from the orb along the jet (16.16) */
    s32 speed;             /* +$04: its speed (16.16; += the jet block's y accel each frame) */
    u16 phase;             /* +$08: +$80 a frame; negative = the start delay; >> 8 = the frame pair */
} GasPuff;
static_assert(offsetof(GasPuff, speed) == 0x04, "GasPuff.speed");
static_assert(offsetof(GasPuff, phase) == 0x08, "GasPuff.phase");
static_assert(sizeof(GasPuff) == 0x0A, "GasPuff");

/* a gas orb (a Vm2Obj, body $041E4A): its three jets' puff blocks and the puff sprite frame */
typedef struct GasOrbObj {
    u8  unk_00[0x12];
    s16 jet[3];            /* +$12/$14/$16: handles of the jets' puff blocks (GasPuff x 5), 0 = off */
    u8  unk_18[0x12];
    u16 puff_frame;        /* +$2A: the puff sprite frame (S4Obj.sprite_frame) */
} GasOrbObj;
static_assert(offsetof(GasOrbObj, jet) == 0x12, "GasOrbObj.jet");
static_assert(offsetof(GasOrbObj, puff_frame) == 0x2A, "GasOrbObj.puff_frame");
