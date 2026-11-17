#include "game.h"

/* the arena's 8 debris effect scripts (a table of a4 script longs, ROM) */
static constexpr uint32_t Scr4_024360 = 0x024360u;

static constexpr int16_t ARENA_PIVOT_X = 0xA8;         /* the pivot (16.16 integer part) at rest */
static constexpr int16_t ARENA_FLOOR_Y = 0xC4;
static constexpr int16_t ARENA_PICTURE_X = -0x46;    /* the hull picture's start */
static constexpr int16_t ARENA_PICTURE_Y = -0x12C;
static constexpr uint16_t ARENA_TILT_WARNING = 4;      /* frames of rumble before the floor turns */
static constexpr int16_t ARENA_SLOPE_RIGHT = 0x1800;   /* the slope targets and their pivots */
static constexpr int16_t ARENA_PIVOT_RIGHT = 0x118;
static constexpr int16_t ARENA_SLOPE_LEFT = -0x1800;
static constexpr int16_t ARENA_PIVOT_LEFT = 0x28;
static constexpr int16_t ARENA_SLOPE_ACCEL = 2;
static constexpr uint8_t ARENA_DEBRIS_RATE = 0x24;     /* an effect on each carry of the timer */
static constexpr uint16_t ARENA_DEBRIS_KINDS = 0x20;   /* Rand_Below: 8 scripts x 4 bytes */
static constexpr uint16_t ARENA_DEBRIS_SPREAD = 0x50;
static constexpr uint32_t ARENA_DEBRIS_Y = 0xE00000u;  /* 224 px below the camera top, 16.16 */
static constexpr int32_t ARENA_ROLL_ACCEL = 0x2000;
static constexpr int32_t ARENA_ROLL_MAX = 0x30000;
static constexpr int32_t ARENA_PIVOT_ACCEL = 0x1200;

/* `move.w #v,long`: the integer part of a 16.16 long, its fraction kept */
static void set_whole(s32 &fixed, int16_t v) { fixed = (int32_t)(((uint32_t)(uint16_t)v << 16) | ((uint32_t)fixed & 0xFFFFu)); }
static int16_t whole(const s32 &fixed) { return (int16_t)((uint32_t)fixed >> 16); }

/* Stage2_LiftArriveTop - turns the lift section into the stage 2 boss arena when the lift reaches
 * the top: swaps the V-int object's handler to Stage2_ArenaVIntScroll (with the V-int held off),
 * sets arena mode, the tilting-floor terrain routines and the arena's pivot and floor values, then
 * spawns the boss
 * In:  the registers for Boss_Stage2_Spawn (d4/d5 its position)
 * Out: g_SceneVars arena = 1; g_TerrainFn/Alt = Coll_Stage2TiltFloor/Rest; the pivot target and x
 *      $A8, floor y $C4, the hull picture at (-$46, -$12C); a0 = the V-int object; then as Boss_Stage2_Spawn
 * Evidence: addq.b #1,g_VIntOff / movea.w g_SceneVars2+$24,a0 / move.l #$2456A,(a0) / move.w
 *           #1,g_SceneVars / subq.b #1,g_VIntOff / move.l #$247C6 / #$247F8 to g_TerrainFn/Alt / the
 *           five word stores ($A8 to +8 twice) / jmp Boss_Stage2_Spawn; only caller
 *           Stage2_LiftUpdate; TAS f32065
 */
void Stage2_LiftArriveTop(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a2,
                          uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    Stage2Vars2 *arena = g_Stage2Vars2;
    g_VIntOff = (uint8_t)(g_VIntOff + 1);     /* no V-int while its handler changes */
    PTR(u32) handler;
    handler.v = (uint32_t)(int32_t)arena->vint_obj;   /* movea.w: the V-int object's handler long */
    *handler = at_Stage2_ArenaVIntScroll;
    g_Stage2Vars->arena = 1;
    g_VIntOff = (uint8_t)(g_VIntOff - 1);
    g_TerrainFn = at_Coll_Stage2TiltFloor;
    g_TerrainFnAlt = at_Coll_Stage2TiltFloorRest;
    set_whole(arena->picture_x, ARENA_PICTURE_X);
    set_whole(arena->picture_y, ARENA_PICTURE_Y);
    set_whole(arena->pivot_x, ARENA_PIVOT_X);     /* written twice in the original */
    set_whole(arena->pivot_target, ARENA_PIVOT_X);
    set_whole(arena->floor_y, ARENA_FLOOR_Y);
    dk::jump(at_Boss_Stage2_Spawn);
}

/* Stage2_ArenaRequestTilt - boss-script native: when the arena is live (g_Stage2TiltAllowed) and the
 * floor is not already turning (roll_speed 0), arms the next floor tilt in 4 frames and resets the
 * debris timer
 * In:  -
 * Out: tilt_countdown = 4 and g_Stage2DebrisTimer = 0 when accepted; the condition codes of the
 *      last test (Z clear: refused) or of the clr.b (Z set)
 * Evidence: tst.b g_Stage2TiltAllowed / beq / tst.l g_SceneVars2+$12 / beq / rts / move.w
 *           #4,g_SceneVars2+$22 / clr.b g_Stage2DebrisTimer; dc.l at $026B58 and $026C52 in
 *           Scr4_026A46; the continue/game-over scenarios and fuzz-25200..32400
 */
void Stage2_ArenaRequestTilt(dk::Flags *ccr)
{
    Stage2Vars2 *arena = g_Stage2Vars2;
    *ccr = dk::Flags::test(g_Stage2TiltAllowed, 1, ccr->x);
    if (g_Stage2TiltAllowed == 0)
        return;
    *ccr = dk::Flags::test((uint32_t)arena->roll_speed, 4, ccr->x);
    if (arena->roll_speed != 0)
        return;
    arena->tilt_countdown = ARENA_TILT_WARNING;
    g_Stage2DebrisTimer = 0;
    *ccr = dk::Flags::test(0, 1, ccr->x);
}

/* Stage2_LiftInstallVInt - puts the lift's scroll handler Stage2_LiftVIntScroll on the V-int object
 * list and keeps that object's handle (Stage2_LiftArriveTop later swaps its handler)
 * In:  a6 (VIntObj_New replaces it)
 * Out: g_SceneVars2 vint_obj = the V-int object
 * Evidence: lea Stage2_LiftVIntScroll,a0 / jsr VIntObj_New / move.w a6,g_SceneVars2+$24; called once by
 *           the scene $32F14 at its start (TAS f21125)
 */
void Stage2_LiftInstallVInt(uint32_t a6)
{
    uint32_t d0 = 0, a1 = 0, obj = a6;
    VIntObj_New(&d0, at_Stage2_LiftVIntScroll, &a1, &obj);
    g_Stage2Vars2->vint_obj = (int16_t)(uint16_t)obj;
}

/* the arena floor's tilt for one frame (Stage2_UpdateScroll in arena mode, up to the pivot) */
static void arena_tilt(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t d7,
                       uint32_t a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    Stage2Vars *vars = g_Stage2Vars;
    Stage2Vars2 *arena = g_Stage2Vars2;
    uint32_t a0 = 0;
    if (arena->tilt_countdown != 0) {
        arena->tilt_countdown = (uint16_t)(arena->tilt_countdown - 1);
        if (arena->tilt_countdown == 0) {     /* the warning is over: turn the other way */
            Player_SetBits300All(&a0);
            arena->pivot_vel = 0;
            vars->slope_speed = 0;            /* clr.l g_SceneVars+2: and the word after it */
            vars->unk_04 = 0;
            if (vars->slope_target < 0) {
                vars->slope_target = ARENA_SLOPE_RIGHT;
                set_whole(arena->pivot_target, ARENA_PIVOT_RIGHT);
            } else {
                vars->slope_target = ARENA_SLOPE_LEFT;
                set_whole(arena->pivot_target, ARENA_PIVOT_LEFT);
            }
        } else {
            Shake_Rumble(d1, d2);
        }
    }

    int16_t slope = vars->slope;
    int16_t target = vars->slope_target;
    *d2 = (*d2 & 0xFFFF0000u) | (uint16_t)slope;
    *d3 = (*d3 & 0xFFFF0000u) | (uint16_t)target;
    if (target != slope) {
        int16_t speed = vars->slope_speed;
        bool moving;
        if (target >= 0) {
            speed = (int16_t)(speed + ARENA_SLOPE_ACCEL);
            moving = slope < target;
        } else {
            speed = (int16_t)(speed - ARENA_SLOPE_ACCEL);
            moving = slope > target;
        }
        *d1 = (*d1 & 0xFFFF0000u) | (uint16_t)speed;
        if (!moving) {                        /* arrived: a big shake */
            arena->roll_speed = 0;
            vars->slope = target;
            Shake_StartBig();
            Player_ClearBits300All(&a0);
        } else {
            slope = (int16_t)(slope + speed);
            *d2 = (*d2 & 0xFFFF0000u) | (uint16_t)slope;
            vars->slope_speed = speed;
            vars->slope = slope;
            Shake_Rumble(d1, d2);
            unsigned timer = (unsigned)g_Stage2DebrisTimer + ARENA_DEBRIS_RATE;
            g_Stage2DebrisTimer = (uint8_t)timer;
            if (timer > 0xFF) {               /* addi.b's carry: a debris effect */
                *d0 = (*d0 & 0xFFFF0000u) | ARENA_DEBRIS_KINDS;
                Rand_Below(d0);
                *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)(*d0 & 0x1C);
                PTR(const u32) scripts;
                scripts.v = Scr4_024360 + (uint32_t)(int32_t)(int16_t)*d0;
                *a4 = *scripts;
                uint16_t x = (uint16_t)(ARENA_PIVOT_X - whole(arena->pivot_target) + ARENA_PIVOT_X);
                *d0 = (*d0 & 0xFFFF0000u) | ARENA_DEBRIS_SPREAD;
                Rand_Signed(d0);
                x = (uint16_t)(x + (uint16_t)*d0);
                *d4 = ((uint32_t)x << 16) | (*d4 >> 16);   /* move.w / swap: x integer part */
                *d5 = g_CameraY + ARENA_DEBRIS_Y;
                /* the a4 script runs on every register (d2-d5: radius, heading, x, y) */
                uint32_t r7 = d7, r9 = a1;   /* changed by the script, read by no caller */
                Fx_RunScriptNoParent(d0, d1, d2, d3, d4, d5, d6, &r7, &a0, &r9, a2, a3, a4, a5, a6);
            }
            int32_t roll = arena->roll_speed + ARENA_ROLL_ACCEL;
            if (roll > ARENA_ROLL_MAX)
                roll = ARENA_ROLL_MAX;
            arena->roll_speed = roll;
            /* add.l d0,g_SceneVars2+$E: the long over floor_y's fraction and roll_lo */
            uint32_t acc = (((uint32_t)arena->floor_y & 0xFFFFu) << 16) | arena->roll_lo;
            acc += (uint32_t)roll;
            arena->floor_y = (int32_t)(((uint32_t)arena->floor_y & 0xFFFF0000u) | (acc >> 16));
            arena->roll_lo = (uint16_t)acc;
        }
    }

    /* the pivot eases toward its target, snapping when it passes it */
    *d1 = (uint32_t)arena->pivot_vel;
    *d2 = (uint32_t)arena->pivot_x;
    *d3 = (uint32_t)arena->pivot_target;
    if (*d2 == *d3)
        return;
    if ((int32_t)(*d2 - *d3) < 0) {           /* cmp.l d3,d2 / bmi: left of it */
        *d1 += ARENA_PIVOT_ACCEL;
        *d2 += *d1;
        if ((int32_t)(*d2 - *d3) >= 0) {
            *d1 = 0;
            *d2 = *d3;
        }
    } else {
        *d1 -= ARENA_PIVOT_ACCEL;
        *d2 += *d1;
        if ((int32_t)(*d2 - *d3) < 0) {
            *d1 = 0;
            *d2 = *d3;
        }
    }
    arena->pivot_vel = (int32_t)*d1;
    arena->pivot_x = (int32_t)*d2;
}

/* Stage2_UpdateScroll - per-frame scroll of the lift scene: view y = g_MapScrollY + shake; lift mode
 * scrolls layer A and the shaft columns; arena mode runs the floor tilt (warning rumble, the slope
 * easing toward +-$1800 with rumbling and random debris effects, a big shake on arrival) and eases
 * the pivot x, then draws the tilt
 * In:  the registers the scene loop and the debris scripts pass through
 * Out: g_Stage2ViewY; lift: layer A's pending_y, then as Layer_ScrollStep and Stage2_LiftColumnScroll;
 *      arena: the tilt state, d1/d2/d3 = the pivot speed, x and target, then as Stage2_ArenaDrawTilt
 * Evidence: g_MapScrollY + g_ShakeY -> g_Stage2ViewY / tst g_SceneVars / lift: g_LayerA, sub.w $2(a6),
 *           move.w d0,$6(a6), jsr Layer_ScrollStep, bra.w Stage2_LiftColumnScroll; arena: the
 *           countdown at g_SceneVars2+$22, the slope +6 -> +8, Rand_Below #$20 & $1C into Scr4_024360,
 *           Fx_RunScriptNoParent, the roll speed capped at $30000, the pivot $1200 steps, bra.w
 *           Stage2_ArenaDrawTilt; every frame of the scene loop ($32FD4); TAS f21158-f35416
 */
void Stage2_UpdateScroll(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t d7,
                         uint32_t a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    g_Stage2ViewY = g_MapScrollY + g_ShakeY;
    if (g_Stage2Vars->arena == 0) {
        MapLayer *layer = (MapLayer *)g_LayerA;
        *a6 = dk::addr(layer);
        layer->pending_y = (int16_t)(uint16_t)((uint16_t)(g_Stage2ViewY >> 16) - layer->y);
        uint16_t w4 = (uint16_t)*d4, w6 = (uint16_t)*d6, w7 = (uint16_t)d7;
        uint32_t a0 = 0;
        Layer_ScrollStep(d1, d2, d3, &w4, d5, &w6, &w7, &a0, a2, a3, layer);
        *d4 = (*d4 & 0xFFFF0000u) | w4;
        *d6 = (*d6 & 0xFFFF0000u) | w6;
        dk::jump(at_Stage2_LiftColumnScroll);
        return;
    }
    /* d0 = the view y with the mode in its low word (the debris effect's d0 keeps that high word;
       Shake_Rumble's, which it changes, no caller reads) */
    uint32_t d0 = (g_Stage2ViewY & 0xFFFF0000u) | g_Stage2Vars->arena;
    arena_tilt(&d0, d1, d2, d3, d4, d5, d6, d7, a1, a2, a3, a4, a5, a6);
    dk::jump(at_Stage2_ArenaDrawTilt);
}
