#include "game.h"

/* the road hazard's a2 script (ROM data after the spawner) */
static constexpr uint32_t Scr2_03FB1E = 0x03FB1Eu;

/* Boss_Hatter_SpawnRoadHazard - road schedule spawn kind 0: spawns a road hazard (Scr2_03FB1E,
 * enemy class) that rides the road from its far end toward the players, and puts it at the
 * horizon
 * In:  d4 = the lane's road x, d5 = y (replaced by the road distance); a4 = the boss's VM block
 * Out: a6 = the object, d0 = 0 (Stage3_RoadPlaceAtHorizon); a full pool quits the caller
 *      (Obj_SpawnEnemyOrQuit returns past this)
 * Evidence: lea Scr2_03FB1E,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Stage3_RoadPlaceAtHorizon; the
 *           pointer in Boss_Hatter_RoadCmdSpawn0; TAS f114753/f114833
 */
void Boss_Hatter_SpawnRoadHazard(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a4,
                                 uint32_t *a6)
{
    uint32_t list = 0, motion = 0, sprite = 0;
    Obj_SpawnEnemyOrQuit(d0, d1, d4, d5, d7, &list, Scr2_03FB1E, &motion, &a4, &sprite, a6);
    dk::jump(at_Stage3_RoadPlaceAtHorizon);
}
