/* types/VM2.h - the a2-script VM's records (docs/NOTES.md "a2 script objects"): the object (a6)
 * and its blocks: motion (a3), VM (a4) and sprite (a5). Views of the fields the VM's own code
 * uses; the rest stay unk_XX. */
#pragma once
#include "dk_mem.h"

typedef struct Vm2Block {          /* a4 */
    u8  body_off;                  /* +$00: body-box off count: while it runs Op2_PlaceBodyBoxes / PlaceBodyBox unlink the body boxes instead of placing them, 1 off the count per run; cleared by Obj_SetBodyBoxes */
    u8  delay;                     /* +$01: runs the script waits before its next op (the hit callbacks pause the attacker) */
    u16 body_count;                /* +$02: body boxes in use (Obj_SetBodyBoxes; the boxes it can be struck through) */
    s16 body[3];                   /* +$04: their handles (HitBox) */
    u16 attack_count;              /* +$0A: attack boxes in use (Obj_SetAttackBoxes) */
    s16 attack[2];                 /* +$0C: their handles (HitBox) */
    s16 tracked;                  /* +$10: handle of a block holding a position to aim at (x +$2E, y +$32;
                                      the targeted player record), 0 = none */
    u8  unk_12[0x02];
    u16 counter[3];                /* +$14/$16/$18: loop counters 0-2 (-1 per run) */
    u16 timer[4];                  /* +$1A/$1C/$1E/$20: timers 0-3, counted down by `tick` per run */
    u16 var[4];                    /* +$22..+$28: script variables 0-3 (the var ops; natives fill them) */
    PTR(const u16) resume;         /* +$2A: where VM2_RunObject resumes the script next frame */
    u16 speed;                     /* +$2E: speed factor ($100 = 1.0, times g_TimeScale >> 8) */
    u16 tick;                      /* +$30: speed >> 4, what a timer loses per run */
    u16 tick_sq;                   /* +$32: tick squared */
    u8  unk_34[0x02];
    s16 points;                    /* +$36: score the object is worth */
    PTR(const u16) coroutine[4];   /* +$38/$3C/$40/$44: coroutine slots 0-3 (the script each resumes at) */
} Vm2Block;
static_assert(offsetof(Vm2Block, delay) == 0x01, "Vm2Block.delay");
static_assert(offsetof(Vm2Block, body_count) == 0x02, "Vm2Block.body_count");
static_assert(offsetof(Vm2Block, attack_count) == 0x0A, "Vm2Block.attack_count");
static_assert(offsetof(Vm2Block, tracked) == 0x10, "Vm2Block.tracked");
static_assert(offsetof(Vm2Block, counter) == 0x14, "Vm2Block.counter");
static_assert(offsetof(Vm2Block, timer) == 0x1A, "Vm2Block.timer");
static_assert(offsetof(Vm2Block, var) == 0x22, "Vm2Block.var");
static_assert(offsetof(Vm2Block, resume) == 0x2A, "Vm2Block.resume");
static_assert(offsetof(Vm2Block, tick) == 0x30, "Vm2Block.tick");
static_assert(offsetof(Vm2Block, points) == 0x36, "Vm2Block.points");
static_assert(offsetof(Vm2Block, coroutine) == 0x38, "Vm2Block.coroutine");

/* motion block flags (byte +0) */
enum {
    MOT_ALT_TERRAIN = 1 << 0,      /* collide with g_TerrainFnAlt instead of g_TerrainFn */
    MOT_BOX_A = 1 << 1,            /* terrain box A on */
    MOT_BOX_B = 1 << 2,            /* terrain box B on */
    MOT_VERTICAL = 1 << 3,        /* |dy| >= |dx| to the target point (Mot_SetTarget) */
    MOT_NEGATIVE = 1 << 4,         /* the dominant component is negative */
};

typedef struct Vm2Motion {         /* a3: the motion block (NOTES "a2 script objects") */
    u8  flags;                     /* +$00: bit 0 alt terrain, 1/2 terrain boxes A/B, MOT_VERTICAL, MOT_NEGATIVE */
    u8  stun;                      /* +$01: runs left with the velocity frozen */
    s32 impulse_x;                 /* +$02: decaying impulses added to the velocity */
    s32 impulse_y;                 /* +$06 */
    u16 heading_target;            /* +$0A: target heading 0-$3FF (0 = +x, $100 = +y) */
    s32 heading;                   /* +$0C: current heading, 16.16 (integer part 0-$3FF) */
    s32 turn_rate;                 /* +$10: heading units (16.16) turned per run */
    s16 point_x;                   /* +$14: the target point (world pixels) */
    s16 point_y;                   /* +$16 */
    s32 vel_x;                     /* +$18: x velocity (16.16), steered toward the target speed +$24 */
    s32 vel_y;                     /* +$1C: y velocity (16.16) */
    s32 accel_x;                   /* +$20: the step toward target_x */
    s32 target_x;                  /* +$24: x target speed (the steering moves vel_x toward it) */
    s32 accel_y;                   /* +$28: the step toward target_y */
    s32 target_y;                  /* +$2C: y target speed */
    s32 cruise_x;                  /* +$30: x cruise speed (Op2_Cruise*) */
    s32 cruise_y;                  /* +$34: y cruise speed */
    s32 last_vel_x;                /* +$38: the velocity as the steering tail $4B8A left it, before terrain */
    s32 last_vel_y;                /* +$3C: contact cleared +$18/$1C (Op2_BounceY reads it) */
    s16 box_a_dx;                 /* +$40: terrain box A: offset from the object (MOT_BOX_A) */
    s16 box_a_dy;                  /* +$42 */
    u16 box_a_wall_mask;           /* +$44: wall kinds it collides (Coll_MoveScene d6) */
    u16 box_a_floor_mask;          /* +$46: floor kinds (d7) */
    s16 box_b_dx;                  /* +$48: terrain box B, collided first (MOT_BOX_B) */
    s16 box_b_dy;                  /* +$4A */
    u16 box_b_wall_mask;           /* +$4C */
    u16 box_b_floor_mask;          /* +$4E */
    u16 terrain_code;              /* +$50: the terrain routine's d3 after the last move (high byte tested) */
    u16 wall_hit;                  /* +$52: the last move's wall contact (Coll_MoveScene d6), 0 = none */
    u16 contact;                   /* +$54: terrain contact flags (the terrain routine's d7, stored by the move op
                                      $4CDA): the last move's floor contact, 0 = airborne; any of bits $B =
                                      on terrain (ground, not airborne) */
} Vm2Motion;
static_assert(offsetof(Vm2Motion, contact) == 0x54, "Vm2Motion.contact");
static_assert(offsetof(Vm2Motion, impulse_x) == 0x02, "Vm2Motion.impulse_x");
static_assert(offsetof(Vm2Motion, heading_target) == 0x0A, "Vm2Motion.heading_target");
static_assert(offsetof(Vm2Motion, heading) == 0x0C, "Vm2Motion.heading");
static_assert(offsetof(Vm2Motion, point_x) == 0x14, "Vm2Motion.point_x");
static_assert(offsetof(Vm2Motion, vel_x) == 0x18, "Vm2Motion.vel_x");
static_assert(offsetof(Vm2Motion, vel_y) == 0x1C, "Vm2Motion.vel_y");
static_assert(offsetof(Vm2Motion, target_x) == 0x24, "Vm2Motion.target_x");
static_assert(offsetof(Vm2Motion, target_y) == 0x2C, "Vm2Motion.target_y");
static_assert(offsetof(Vm2Motion, cruise_y) == 0x34, "Vm2Motion.cruise_y");
static_assert(offsetof(Vm2Motion, last_vel_y) == 0x3C, "Vm2Motion.last_vel_y");
static_assert(offsetof(Vm2Motion, box_a_dx) == 0x40, "Vm2Motion.box_a_dx");
static_assert(offsetof(Vm2Motion, box_b_dx) == 0x48, "Vm2Motion.box_b_dx");
static_assert(offsetof(Vm2Motion, terrain_code) == 0x50, "Vm2Motion.terrain_code");

typedef struct Vm2Obj {            /* a6 */
    u32 handler;                   /* +$00: the object's handler, run each frame (code) */
    s16 prev;                      /* +$04: the object list's prev link; 0 = on no list */
    u8  unk_06[0x02];
    s16 motion;                    /* +$08: handle of the motion block (a3) */
    s16 vm;                        /* +$0A: handle of the VM block (a4) */
    s16 sprite;                    /* +$0C: handle of the sprite block (a5) */
    u8  unk_0E[0x04];
    u16 touch_damage;              /* +$12: damage its body does on contact (EnemyAtk_HitTouch) */
    u8  unk_14[0x04];
    u8  population;                /* +$18: population weight (summed in g_ObjPopulation) */
    u8  spawn_action;              /* +$19: Tbl_012EF0 index run by Op2_RunSpawnAction (1 = group member) */
    u16 spawn_arg;                 /* +$1A: its argument (d1) */
    s16 jitter;                    /* +$1C: random offset added to the player-distance windows (Op2_SetRandJitter) */
    u32 player;                    /* +$1E: player slot objects: their PlayerRec (g_Player1 / g_Player2) */
    u8  unk_22[0x10];
    PTR(const u16) vector[3];      /* +$32/$36/$3A: script vectors 0-2 */
    s16 x;                         /* +$3E: world x, integer part (16.16 at +$3E) */
    u16 x_frac;
    s16 y;                         /* +$42: world y, integer part */
    u16 y_frac;
    s32 saved_x;                   /* +$46: a saved copy of the 16.16 position (Op2_SavePos) */
    s32 saved_y;                   /* +$4A */
    s16 draw_x;                    /* +$4E: the drawn position (Obj_SyncChild copies it between parent and child) */
    s16 draw_y;                    /* +$50 */
    s16 energy;                    /* +$52: energy left (Op2_SetEnergy; the boss meter shows it) */
    u16 energy_pct;                /* +$54: 1% of the energy it was given (Op2_BranchEnergyAtPct) */
    u16 serial;                    /* +$56: a serial number from g_ObjSerial (players: copied to the record +$3C) */
    u16 owner_id;                  /* +$58: 1/2 the players, $A/$B enemies (copied into its boxes) */
} Vm2Obj;
static_assert(offsetof(Vm2Obj, prev) == 0x04, "Vm2Obj.prev");
static_assert(offsetof(Vm2Obj, vm) == 0x0A, "Vm2Obj.vm");
static_assert(offsetof(Vm2Obj, population) == 0x18, "Vm2Obj.population");
static_assert(offsetof(Vm2Obj, spawn_arg) == 0x1A, "Vm2Obj.spawn_arg");
static_assert(offsetof(Vm2Obj, jitter) == 0x1C, "Vm2Obj.jitter");
static_assert(offsetof(Vm2Obj, player) == 0x1E, "Vm2Obj.player");
static_assert(offsetof(Vm2Obj, draw_x) == 0x4E, "Vm2Obj.draw_x");
static_assert(offsetof(Vm2Obj, energy) == 0x52, "Vm2Obj.energy");
static_assert(offsetof(Vm2Obj, serial) == 0x56, "Vm2Obj.serial");
static_assert(offsetof(Vm2Obj, vector) == 0x32, "Vm2Obj.vector");
static_assert(offsetof(Vm2Obj, x) == 0x3E, "Vm2Obj.x");
static_assert(offsetof(Vm2Obj, y) == 0x42, "Vm2Obj.y");
static_assert(offsetof(Vm2Obj, saved_x) == 0x46, "Vm2Obj.saved_x");
static_assert(offsetof(Vm2Obj, owner_id) == 0x58, "Vm2Obj.owner_id");

/* sprite block flags (byte +$18) */
enum {
    SPR_ANIM_DONE = 1 << 0,        /* the animation script stopped (Op2_SetAnim clears it) */
};

typedef struct Vm2Sprite {         /* a5 */
    u16 frame[10];                 /* +$00: the frame slots (Spr_ChangeFrame handles) */
    u16 frame_count;               /* +$14: slots in use */
    u8  invulnerable;              /* +$16: frames the object can't be hit (players: $78 after a spawn;
                                      Hitbox_CanHitOwner) */
    u8  shake;                     /* +$17: shake timer: while it runs Op2_Draw displaces the sprite at random (Op2_SetShakeTimer) */
    u8  flags;                     /* +$18: SPR_ANIM_DONE */
    u8  unk_19;
    u16 aim_dir;                   /* +$1A: aim direction x 4 (Op2_AimDirAtPlayer; Op2_SwitchAimDir indexes its table by it) */
    u16 anim_delay;                /* +$1C: display time added to anim_timer per frame (Op2_SetAnimDelay; $100 = one run at speed 1.0) */
    u16 anim_timer;                /* +$1E: runs left before the animation script steps */
    u16 anim_loop;                 /* +$20: the animation loop counter (Op2_SetAnimLoop, counted down by Op2_AnimLoop) */
    s16 var[3];                    /* +$22/$24/$26: sprite variables (Op2_SetSprVarN; natives step them) */
    u8  unk_28[0x02];
    PTR(const u16) draw;           /* +$2A: the draw list: the inline sprite ops of the frame Op2_AnimFrame shows (Op2_Draw runs them) */
    PTR(const u16) anim;           /* +$2E: the animation script */
    s32 frame_bias;                /* +$32: velocity-to-frame mapping: added to the velocity (the jump ops:
                                      -launch speed) before the frame ops $3EE2/$3F1C divide it */
    u16 frame_div;                 /* +$36: ... by this divisor (cruise speed / 100, launch / 50) */
    u16 attr;                     /* +$38: sprite attributes (d6 of Spr_Add*; high byte bit 3 = H flip) */
    u16 depth;                     /* +$3A: sort depth (d7 of Spr_AddSorted) */
} Vm2Sprite;
static_assert(offsetof(Vm2Sprite, frame_count) == 0x14, "Vm2Sprite.frame_count");
static_assert(offsetof(Vm2Sprite, invulnerable) == 0x16, "Vm2Sprite.invulnerable");
static_assert(offsetof(Vm2Sprite, flags) == 0x18, "Vm2Sprite.flags");
static_assert(offsetof(Vm2Sprite, anim_timer) == 0x1E, "Vm2Sprite.anim_timer");
static_assert(offsetof(Vm2Sprite, aim_dir) == 0x1A, "Vm2Sprite.aim_dir");
static_assert(offsetof(Vm2Sprite, anim_delay) == 0x1C, "Vm2Sprite.anim_delay");
static_assert(offsetof(Vm2Sprite, anim_loop) == 0x20, "Vm2Sprite.anim_loop");
static_assert(offsetof(Vm2Sprite, var) == 0x22, "Vm2Sprite.var");
static_assert(offsetof(Vm2Sprite, shake) == 0x17, "Vm2Sprite.shake");
static_assert(offsetof(Vm2Sprite, draw) == 0x2A, "Vm2Sprite.draw");
static_assert(offsetof(Vm2Sprite, anim) == 0x2E, "Vm2Sprite.anim");
static_assert(offsetof(Vm2Sprite, frame_bias) == 0x32, "Vm2Sprite.frame_bias");
static_assert(offsetof(Vm2Sprite, frame_div) == 0x36, "Vm2Sprite.frame_div");
static_assert(offsetof(Vm2Sprite, attr) == 0x38, "Vm2Sprite.attr");
static_assert(offsetof(Vm2Sprite, depth) == 0x3A, "Vm2Sprite.depth");
