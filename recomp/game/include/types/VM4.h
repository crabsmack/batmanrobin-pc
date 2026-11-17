/* types/VM4.h - the a4 effect-script object (a6 of the a4 ops and natives; docs/NOTES.md "a4
 * effect scripts"): one $5A pool block on the main object list, run by VM4_RunObject. The spawn
 * context g_SpawnCtx has the same layout. Fields the C uses; the rest stay unk_XX. */
#pragma once
#include "dk_mem.h"

typedef struct Vm4Obj {
    u8  unk_00[0x08];
    PTR(const u16) resume;  /* +$08: where VM4_RunObject resumes the script next frame */
    PTR(const u16) anim;    /* +$0C: the animation script (Op4_SetAnim) */
    u16 owner_id;           /* +$10: inherited owner id (shots: the player's pad + 2) */
    u16 damage;             /* +$12: damage it deals (Shot_DamageTarget) */
    s16 box;                /* +$14: handle of its collision box, 0 = none */
    s16 timer;              /* +$16: frames (Op4_SetTimer; Op4_TimerBranchOut / Op4_TimerLoop) */
    u8  hit_phase;          /* +$18: the frame parity its hit tests run on (Op4_TakeHitPhaseA / B; Op4_HitPlayersOnPhase compares g_FrameParity) */
    u8  counter;            /* +$19: count byte (Op4_SetCounterTicker; Op4_CounterKillOut / BranchOut) */
    u8  ticker;             /* +$1A: ticker accumulator: fires when ticker + ticker_rate carries */
    u8  ticker_rate;        /* +$1B: added to the accumulator per step (rate/256 per frame) */
    u8  anim_tick;          /* +$1C: animation ticker accumulator */
    u8  anim_rate;          /* +$1D: its rate */
    u16 frame;              /* +$1E: the sprite frame wanted */
    u16 frame_shown;        /* +$20: the frame whose art is loaded (Spr_ReleaseFrame frees it) */
    u16 attr;               /* +$22: sprite attributes (bit 15 = high priority) */
    u16 depth;              /* +$24: 0 front list, 1 back, else sorted */
    /* the four motion values, 16.16 each as an integer and a fraction word (vm4_fix reads the
       long); the drawn point is (x, y) + radius * (cos, sin)(heading) */
    s16 radius;             /* +$26: radius (px) */
    u16 radius_frac;
    s16 heading;            /* +$2A: heading 0-$3FF ($400 a turn) */
    u16 heading_frac;
    s16 x;                  /* +$2E: world x (px) */
    u16 x_frac;
    s16 y;                  /* +$32: world y (px) */
    u16 y_frac;
    s32 vel_radius;         /* +$36: velocities, 16.16 per frame (the motion op $6286 adds them) */
    s32 vel_heading;        /* +$3A */
    s32 vel_x;              /* +$3E */
    s32 vel_y;              /* +$42 */
    s32 acc_radius;         /* +$46: accelerations, 16.16, added to the velocities */
    s32 acc_heading;        /* +$4A */
    s32 acc_x;              /* +$4E */
    s32 acc_y;              /* +$52 */
    s16 draw_x;             /* +$56: the drawn point's pixel words (Op4_SaveDrawPos) */
    s16 draw_y;             /* +$58 */
} Vm4Obj;
static_assert(offsetof(Vm4Obj, resume) == 0x08, "Vm4Obj.resume");
static_assert(offsetof(Vm4Obj, anim) == 0x0C, "Vm4Obj.anim");
static_assert(offsetof(Vm4Obj, owner_id) == 0x10, "Vm4Obj.owner_id");
static_assert(offsetof(Vm4Obj, damage) == 0x12, "Vm4Obj.damage");
static_assert(offsetof(Vm4Obj, box) == 0x14, "Vm4Obj.box");
static_assert(offsetof(Vm4Obj, timer) == 0x16, "Vm4Obj.timer");
static_assert(offsetof(Vm4Obj, counter) == 0x19, "Vm4Obj.counter");
static_assert(offsetof(Vm4Obj, ticker) == 0x1A, "Vm4Obj.ticker");
static_assert(offsetof(Vm4Obj, anim_tick) == 0x1C, "Vm4Obj.anim_tick");
static_assert(offsetof(Vm4Obj, frame) == 0x1E, "Vm4Obj.frame");
static_assert(offsetof(Vm4Obj, frame_shown) == 0x20, "Vm4Obj.frame_shown");
static_assert(offsetof(Vm4Obj, attr) == 0x22, "Vm4Obj.attr");
static_assert(offsetof(Vm4Obj, depth) == 0x24, "Vm4Obj.depth");
static_assert(offsetof(Vm4Obj, radius) == 0x26, "Vm4Obj.radius");
static_assert(offsetof(Vm4Obj, heading) == 0x2A, "Vm4Obj.heading");
static_assert(offsetof(Vm4Obj, x) == 0x2E, "Vm4Obj.x");
static_assert(offsetof(Vm4Obj, y) == 0x32, "Vm4Obj.y");
static_assert(offsetof(Vm4Obj, vel_radius) == 0x36, "Vm4Obj.vel_radius");
static_assert(offsetof(Vm4Obj, vel_y) == 0x42, "Vm4Obj.vel_y");
static_assert(offsetof(Vm4Obj, acc_radius) == 0x46, "Vm4Obj.acc_radius");
static_assert(offsetof(Vm4Obj, acc_y) == 0x52, "Vm4Obj.acc_y");
static_assert(offsetof(Vm4Obj, draw_x) == 0x56, "Vm4Obj.draw_x");
static_assert(sizeof(Vm4Obj) == 0x5A, "Vm4Obj size");

/* a 16.16 motion value of Vm4Obj as one long, and storing / adding to one */
static inline uint32_t vm4_fix(const s16 &whole, const u16 &frac)
{
    return (uint32_t)(uint16_t)whole << 16 | (uint16_t)frac;
}
static inline void vm4_set_fix(s16 &whole, u16 &frac, uint32_t v)
{
    whole = (int16_t)(v >> 16);
    frac = (uint16_t)v;
}
static inline void vm4_add_fix(s16 &whole, u16 &frac, uint32_t d)
{
    vm4_set_fix(whole, frac, vm4_fix(whole, frac) + d);
}
/* add.l to a velocity / acceleration long, wrapping like the 68000 */
static inline void vm4_add_long(s32 &v, uint32_t d)
{
    v = (int32_t)((uint32_t)(int32_t)v + d);
}
