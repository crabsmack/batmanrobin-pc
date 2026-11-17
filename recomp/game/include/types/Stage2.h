/* types/Stage2.h - stage 2's per-scene scratch blocks as the lift and arena code use them
 * (docs/NOTES.md "Stage 2 lift and boss arena"): g_SceneVars ($F4F4..) and g_SceneVars2
 * ($F59E-$F5DD) while the arena runs. */
#pragma once
#include "dk_mem.h"

typedef struct Stage2Vars {         /* g_SceneVars in stage 2 */
    u16 arena;                      /* +$00: 0 the lift, 1 the boss arena (Stage2_LiftArriveTop) */
    s16 slope_speed;                /* +$02: the slope's speed (+-2 steps, Stage2_UpdateScroll) */
    u16 unk_04;                     /* +$04: cleared with slope_speed (clr.l) */
    s16 slope;                      /* +$06: the arena floor's tilt, eased toward +$08 (+-$1800);
                                       Boss_Stage2_TiltPoint shears hull points by it (muls) */
    s16 slope_target;               /* +$08: +-$1800, flipped by each tilt */
    s16 col_a[20];                  /* +$0A: plane A's 20 column V-scrolls (the shaft's depth
                                       bands, Stage2_LiftColumnScroll; the arena floor) */
    s16 col_b[20];                  /* +$32: plane B's column V-scrolls above the arena split */
    s16 col_b_lower[20];            /* +$5A: plane B's below the split, as drawn */
    s16 col_b_split[20];            /* +$82: the copy the split H-int loads (Stage2_ArenaHIntSplit),
                                       kept by the V-int while the next frame is drawn */
} Stage2Vars;
static_assert(offsetof(Stage2Vars, slope) == 0x06, "Stage2Vars.slope");
static_assert(offsetof(Stage2Vars, slope_speed) == 0x02, "Stage2Vars.slope_speed");
static_assert(offsetof(Stage2Vars, slope_target) == 0x08, "Stage2Vars.slope_target");
static_assert(offsetof(Stage2Vars, col_a) == 0x0A, "Stage2Vars.col_a");
static_assert(offsetof(Stage2Vars, col_b) == 0x32, "Stage2Vars.col_b");
static_assert(offsetof(Stage2Vars, col_b_lower) == 0x5A, "Stage2Vars.col_b_lower");
static_assert(offsetof(Stage2Vars, col_b_split) == 0x82, "Stage2Vars.col_b_split");

typedef struct Stage2Vars2 {        /* g_SceneVars2 in stage 2 */
    s32 pivot_target;               /* +$00: the pivot x the tilt eases to, 16.16 ($A8 at rest, $118 / $28
                                       for the two tilts; Stage2_UpdateScroll) */
    s32 pivot_vel;                  /* +$04: the pivot's speed ($1200 steps) */
    s32 pivot_x;                    /* +$08: the tilt's pivot x, 16.16 */
    s32 floor_y;                    /* +$0C: the floor's y, 16.16 (the tilt's x shear is about it) */
    u16 roll_lo;                    /* +$10: with floor_y's fraction word, the long Stage2_UpdateScroll
                                       adds the turn speed to (add.l at +$E); Stage2_ArenaDrawTilt
                                       starts plane A's column ramp from it */
    s32 roll_speed;                 /* +$12: the arena floor's turn speed, nonzero while it turns
                                       (Stage2_UpdateScroll; the boss's phase-2 waits test it) */
    s32 picture_x;                  /* +$16: the hull picture's x, 16.16 (the boss's x word,
                                       Boss_Stage2_MoveHullRunTurrets) */
    s32 picture_y;                  /* +$1A: its y, 16.16 */
    u16 split_next;                 /* +$1E: the arena split line as drawn */
    u16 split_line;                 /* +$20: the copy the V-int arms (low byte = VDP reg $A, the
                                       H-int line; 0 = no split: plane B's table stays) */
    u16 tilt_countdown;             /* +$22: frames of warning rumble before a tilt (Stage2_ArenaRequestTilt) */
    s16 vint_obj;                   /* +$24: handle of the section's V-int object (Stage2_LiftInstallVInt) */
    u16 floor_line;                 /* +$26: the lift: the H-int line of the platform's top, $AF - shake */
    u16 shake_vscroll[2];           /* +$28: the lift: the platform's column V-scroll (the shake), for the H-int */
} Stage2Vars2;
static_assert(offsetof(Stage2Vars2, split_line) == 0x20, "Stage2Vars2.split_line");
static_assert(offsetof(Stage2Vars2, pivot_vel) == 0x04, "Stage2Vars2.pivot_vel");
static_assert(offsetof(Stage2Vars2, pivot_x) == 0x08, "Stage2Vars2.pivot_x");
static_assert(offsetof(Stage2Vars2, floor_y) == 0x0C, "Stage2Vars2.floor_y");
static_assert(offsetof(Stage2Vars2, roll_lo) == 0x10, "Stage2Vars2.roll_lo");
static_assert(offsetof(Stage2Vars2, roll_speed) == 0x12, "Stage2Vars2.roll_speed");
static_assert(offsetof(Stage2Vars2, picture_x) == 0x16, "Stage2Vars2.picture_x");
static_assert(offsetof(Stage2Vars2, picture_y) == 0x1A, "Stage2Vars2.picture_y");
static_assert(offsetof(Stage2Vars2, tilt_countdown) == 0x22, "Stage2Vars2.tilt_countdown");
static_assert(offsetof(Stage2Vars2, vint_obj) == 0x24, "Stage2Vars2.vint_obj");
static_assert(offsetof(Stage2Vars2, floor_line) == 0x26, "Stage2Vars2.floor_line");
static_assert(offsetof(Stage2Vars2, shake_vscroll) == 0x28, "Stage2Vars2.shake_vscroll");

typedef struct Stage2BossObj {      /* a6 of the arena boss (Scr2_025BEE): a Vm2Obj with its turrets */
    u8  unk_00[0x1E];
    s16 turret[4];                  /* +$1E/$20/$22/$24: the four unlinked turret objects (handles,
                                       Op2_SpawnChildOrWait), left to right */
    u8  unk_26[0x18];
    s16 x;                          /* +$3E: x, 16.16 (as Vm2Obj) */
    u16 x_frac;
    s32 y;                          /* +$42: y, 16.16 (the hull sinks as the floor rolls) */
} Stage2BossObj;
static_assert(offsetof(Stage2BossObj, turret) == 0x1E, "Stage2BossObj.turret");
static_assert(offsetof(Stage2BossObj, x) == 0x3E, "Stage2BossObj.x");
static_assert(offsetof(Stage2BossObj, y) == 0x42, "Stage2BossObj.y");

/* the blocks as structs (stage 2 gives g_SceneVars / g_SceneVars2 these layouts) */
#define g_Stage2Vars ((Stage2Vars *)g_SceneVars)
#define g_Stage2Vars2 ((Stage2Vars2 *)g_SceneVars2)
