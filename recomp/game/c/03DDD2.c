#include "game.h"

enum {
    ROAD_FAR_DISTANCE = 0x8C0,   /* the road's far end (Boss_Hatter_RoadHazardPlace kills at it) */
};

/* Stage3_RoadPlaceAtHorizon - puts a Hatter-road object at the road's far end: its road distance
 * (its y field) = $8C0 - the checker phase + this frame's speed, so it lands on a row boundary
 * once the road has moved
 * In:  obj = the object (a6)
 * Out: obj->y; d0 = 0
 * Evidence: move.w #$8C0,d0 / sub.w g_SceneVars2+$E / add.w g_SceneVars2+$C / move.w d0,$42(a6) /
 *           moveq #0,d0; jmp from Boss_Hatter_SpawnRoadHazard; TAS f114753/f114833
 */
uint32_t Stage3_RoadPlaceAtHorizon(Vm2Obj *obj)
{
    const HatterRoadVars *road = hatter_road();
    obj->y = (int16_t)(ROAD_FAR_DISTANCE - road->phase + road->step);
    return 0;
}
