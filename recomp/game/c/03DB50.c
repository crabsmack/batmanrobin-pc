#include "game.h"

/* the road-end swirl's a2 script (ROM data after this file's code) */
static constexpr uint32_t Scr2_03DBC6 = 0x03DBC6u;

/* The road steerer keeps its speed target and easing rate in its script variables as two
 * overlapping longs: the target at var 0-1 (+$22) and the rate at var 1-2 (+$24), so setting the
 * rate overwrites the target's fraction word. */
static int32_t steer_target(const Vm2Block *steer)
{
    return (int32_t)(((uint32_t)steer->var[0] << 16) | steer->var[1]);
}
static int32_t steer_rate(const Vm2Block *steer)
{
    return (int32_t)(((uint32_t)steer->var[1] << 16) | steer->var[2]);
}

/* Boss_Hatter_SetRoadSpeed - a2 native (2 words): sets the road speed the steerer eases to: word 0
 * = the target speed (integer, as 16.16), word 1 = the easing rate per frame in 1/256 (it
 * overwrites the target's fraction word)
 * In:  script = the two words (a2); steer = the steerer's VM block (a4)
 * Out: steer var 0-2; script past the words; d0 = the rate (long); the condition codes of its
 *      move.l
 * Evidence: moveq #0,d0 / move.w (a2)+,d0 / swap / move.l d0,$22(a4) / move.w (a2)+,d0 / ext.l /
 *           asl.l #8 / move.l d0,$24(a4); dc.l $0003DB50 in Scr2_03DA8C (FFE8 0020, 0000 0008)
 *           and Scr2_03F27E; TAS f114542-f115354
 */
void Boss_Hatter_SetRoadSpeed(uint32_t *d0, const u16 **script, Vm2Block *steer, dk::Flags *ccr)
{
    const u16 *w = *script;
    uint16_t target = w[0];
    steer->var[0] = target;                         /* move.l: integer word, 0 fraction */
    steer->var[1] = 0;
    uint32_t rate = (uint32_t)((int32_t)(int16_t)w[1] << 8);
    steer->var[1] = (uint16_t)(rate >> 16);
    steer->var[2] = (uint16_t)rate;
    *script = w + 2;
    *d0 = rate;
    *ccr = dk::Flags::test(rate, 4, ccr->x);
}

/* Boss_Hatter_SteerRoad - the steerer's per-frame native: eases the road speed toward the target
 * by the rate (stopping at it), and publishes the steerer's world position as the road's bend =
 * x / 16 and pitch = y / 16 (Stage3_DrawRoad clamps them)
 * In:  steer = the steerer's VM block (a4: target, rate); self = the steerer (a6: x/y 16.16)
 * Out: hatter_road() speed, bend, pitch; d0 = the pitch, d1 = the rate, d2 = the target; the
 *      condition codes of the pitch's move.l
 * Evidence: move.l g_SceneVars2+8,d0 / $24(a4),d1 / $22(a4),d2 / cmp.l / bge sub / add / ble /
 *           clamp; move.l $3E(a6),d0 / asr.l #4 / +$16; $42(a6) / asr.l #4 / +$1C; dc.l $0003DB66
 *           at $03DAB4 in the steerer's frame loop; TAS f113312-f116145
 */
void Boss_Hatter_SteerRoad(uint32_t *d0, uint32_t *d1, uint32_t *d2, const Vm2Block *steer, const Vm2Obj *self,
                           dk::Flags *ccr)
{
    HatterRoadVars *road = hatter_road();
    int32_t speed = road->speed;
    int32_t rate = steer_rate(steer);
    int32_t target = steer_target(steer);
    *d1 = (uint32_t)rate;
    *d2 = (uint32_t)target;
    if (speed < target) {
        speed = (int32_t)((uint32_t)speed + (uint32_t)rate);
        if (speed > target)
            speed = target;
    } else {
        speed = (int32_t)((uint32_t)speed - (uint32_t)rate);
        if (speed < target)
            speed = target;
    }
    road->speed = speed;
    int32_t x = (int32_t)(((uint32_t)(uint16_t)self->x << 16) | self->x_frac);
    int32_t y = (int32_t)(((uint32_t)(uint16_t)self->y << 16) | self->y_frac);
    road->bend = x >> 4;
    road->pitch = y >> 4;
    *d0 = (uint32_t)road->pitch;
    *ccr = dk::Flags::test(*d0, 4, ccr->x);
}

/* Stage3_RunRoadEndSwirl - runs the road-end swirl (an unlinked object kept in g_SceneVars)
 * through VM2_RunObject after the object list, so it is placed on this frame's road rows; nothing
 * when there is none
 * In:  hatter_road_scene()->swirl
 * Out: d0's low word = the handle; with a swirl a6 = it, then VM2_RunObject's
 * Evidence: move.w (g_SceneVars).w,d0 / beq rts / movea.w d0,a6 / jmp VM2_RunObject; jsr from
 *           Stage3_RunHatterRoad's frame loop after Obj_RunList; TAS f113312-f116145
 */
void Stage3_RunRoadEndSwirl(uint16_t *d0, uint32_t *a0, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5,
                            uint32_t *a6)
{
    int16_t swirl = hatter_road_scene()->swirl;
    *d0 = (uint16_t)swirl;
    if (swirl == 0)
        return;
    *a6 = (uint32_t)(int32_t)swirl;
    dk::jump(at_VM2_RunObject);
}

/* Stage3_SpawnRoadEndSwirl - spawns the pink swirl at the road's far end (Scr2_03DBC6, an unlinked
 * enemy-class object placed each frame by Stage3_PlaceRoadEndSwirl) and keeps it for
 * Stage3_RunRoadEndSwirl
 * In:  d4/d5 = position (unused by the swirl: it is placed by the road)
 * Out: hatter_road_scene()->swirl = a6 = the object, a3/a4 = its blocks; a full pool quits the
 *      caller (Obj_SpawnEnemyUnlinkedOrQuit returns past this)
 * Evidence: lea Scr2_03DBC6,a2 / jsr Obj_SpawnEnemyUnlinkedOrQuit / move.w a6,(g_SceneVars).w /
 *           moveq #0,d0 / rts; jsr from Stage3_InitHatterRoad; png f113400: a pink swirl where
 *           the road begins
 */
void Stage3_SpawnRoadEndSwirl(uint16_t d4, uint16_t d5, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    uint32_t d0 = 0, d7 = 0, list = 0, sprite = 0;
    uint16_t d1 = 0;
    Obj_SpawnEnemyUnlinkedOrQuit(&d0, &d1, d4, d5, &d7, &list, Scr2_03DBC6, a3, a4, &sprite, a6);
    hatter_road_scene()->swirl = (int16_t)(uint16_t)*a6;
}
