#include "game.h"

enum {
    ROAD_NEAR_DISTANCE = 0x840,   /* Stage3_RoadPlaceAtHorizon's $8C0 less $80: one checker row nearer */
};

/* Stage3_RoadPlaceNear - puts a Hatter-road object one checker row nearer than the horizon: its road
 * distance (its y field) = $840 - the checker phase + this frame's speed, so it lands on a row
 * boundary once the road has moved
 * In:  obj = the object (a6)
 * Out: obj->y; d0 = 0
 * Evidence: move.w #$840,d0 / sub.w g_SceneVars2+$E / add.w g_SceneVars2+$C / move.w d0,$42(a6) /
 *           moveq #0,d0 / rts (Stage3_RoadPlaceAtHorizon's code with $840; labelled Scr2_03DDE6 until
 *           2026-10-30); jmp from Boss_Hatter_SpawnRoadHazardNear; no corpus run reaches it (raw-stubs
 *           2026-10-30); the contract is Stage3_RoadPlaceAtHorizon's; 03DDE6.syn checks it
 */
uint32_t Stage3_RoadPlaceNear(Vm2Obj *obj)
{
    const HatterRoadVars *road = hatter_road();
    obj->y = (int16_t)(ROAD_NEAR_DISTANCE - road->phase + road->step);
    return 0;
}
