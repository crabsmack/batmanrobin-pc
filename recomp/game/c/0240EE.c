#include "game.h"

static constexpr uint32_t LIFT_UP_SPEED = 0xFFFF0000u;    /* -1.0 px a frame (16.16) */
static constexpr uint32_t LIFT_START_SPEED = 0xFFFFE000u; /* -1/8 px a frame */
static constexpr int32_t LIFT_ACCEL = 0x300;              /* the speed's step toward the goal */
static constexpr int32_t LIFT_STOP_BELOW = 0x4000;        /* braking under 0.25 px a frame stops it */

/* Stage2_LiftStop - lift trigger callback of the zeppelin loading dock: the lift's goal speed 0,
 * so Stage2_LiftUpdate brakes it to a stop with a jolt
 * In:  d4/d5 = the trigger x/y (Trigger_RunY)
 * Out: g_LiftGoalVelY = 0; d0 = 0
 * Evidence: clr.l g_LiftGoalVelY / moveq #0,d0 / rts; dc.l entries in the y-keyed trigger table
 *           Tbl_02105A ($021754, $021826, ...); TAS f21729-f29582 (5 stops)
 */
uint32_t Stage2_LiftStop(void)
{
    g_LiftGoalVelY = 0;
    return 0;
}

/* Stage2_LiftStart - starts the zeppelin loading-dock lift upward: goal speed -1.0 px a frame, the
 * speed itself -1/8
 * In:  -
 * Out: g_LiftGoalVelY = -$10000, g_LiftVelY = -$2000; d0 = 0
 * Evidence: move.l #$FFFF0000,g_LiftGoalVelY / move.l #$FFFFE000,g_LiftVelY / moveq #0,d0; called by
 *           the scene $32F14 at its start and by lift trigger entries in Tbl_02105A
 */
uint32_t Stage2_LiftStart(void)
{
    g_LiftGoalVelY = LIFT_UP_SPEED;
    g_LiftVelY = LIFT_START_SPEED;
    return 0;
}

/* Stage2_LiftUpdate - per-frame lift motion of the zeppelin loading dock: steps g_LiftVelY by $300
 * toward the goal speed (braking to 0 with a Shake_Start jolt once under 0.25 px a frame), adds it
 * to g_MapScrollY and, when the map reaches the top, turns the section into the boss arena
 * In:  d4/d5 and the rest for Stage2_LiftArriveTop
 * Out: d0 = the speed (0 after a stop), d1 = the goal, d2 = $300; g_LiftVelY, g_MapScrollY. At the
 *      top: Stage2_LiftArriveTop, Shake_Start, the scroll, the speed and the goal cleared
 * Evidence: move.l #$300,d2 / the goal's sign picks sub.l / add.l with a cmp.l clamp; goal 0: toward
 *           0, cmpi.l #$FFFFC000 / #$4000 then jsr Shake_Start / moveq #0,d0; add.l d0,g_MapScrollY /
 *           bpl / jsr Stage2_LiftArriveTop / jsr Shake_Start / 3 x clr.l; every frame of the scene loop
 *           $32FBC; TAS f21158-f32065
 */
void Stage2_LiftUpdate(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a2,
                       uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *d2 = (uint32_t)LIFT_ACCEL;
    *d0 = g_LiftVelY;
    *d1 = g_LiftGoalVelY;
    int32_t goal = (int32_t)*d1;
    bool store = true;
    if (goal > 0) {
        *d0 += *d2;
        if ((int32_t)*d0 > goal)      /* cmp.l d1,d0 / ble */
            *d0 = *d1;
    } else if (goal < 0) {
        *d0 -= *d2;
        if ((int32_t)*d0 < goal)      /* cmp.l d1,d0 / bge */
            *d0 = *d1;
    } else if (*d0 == 0) {
        store = false;                /* stopped: nothing to brake */
    } else {
        bool stops;
        if ((int32_t)*d0 < 0) {
            *d0 += *d2;
            stops = (int32_t)*d0 >= -LIFT_STOP_BELOW;   /* cmpi.l #$FFFFC000 / blt */
        } else {
            *d0 -= *d2;
            stops = (int32_t)*d0 <= LIFT_STOP_BELOW;    /* cmpi.l #$4000 / bgt */
        }
        if (stops) {                  /* the jolt as it halts */
            Shake_Start();
            *d0 = 0;
        }
    }
    if (store)
        g_LiftVelY = *d0;
    g_MapScrollY = g_MapScrollY + *d0;
    if ((int32_t)(uint32_t)g_MapScrollY >= 0)   /* add.l / bpl */
        return;
    uint16_t w1 = (uint16_t)*d1;
    Stage2_LiftArriveTop(d0, &w1, d4, d5, d7, a2, a3, a4, a5, a6);
    *d1 = (*d1 & 0xFFFF0000u) | w1;
    Shake_Start();
    g_MapScrollY = 0;
    g_LiftVelY = 0;
    g_LiftGoalVelY = 0;
}
