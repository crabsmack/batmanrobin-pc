#include "game.h"

/* Boss_Hatter_RoadCmdSpawn0 - road schedule spawn kind 0: picks Boss_Hatter_SpawnRoadHazard, then
 * the spawn commands' common tail (the seven raw stubs after it pick the never-run kinds 1-7)
 * In:  a0/d4/d1 as Boss_Hatter_RoadCmdTbl; a4 = the boss's VM block
 * Out: a1 = the spawner; then Boss_Hatter_RoadCmdSpawnInLane's
 * Evidence: lea Boss_Hatter_SpawnRoadHazard,a1 / bra.s Boss_Hatter_RoadCmdSpawnInLane; TAS
 *           f114753-f114833. a1 an output (measured as a clobber): the tail it branches to calls
 *           through it
 */
void Boss_Hatter_RoadCmdSpawn0(uint32_t *d0, uint16_t *d1, uint32_t *d4, uint16_t *d5, uint32_t *d7, uint32_t a0,
                               uint32_t *a1, uint32_t a4, uint32_t a6)
{
    *a1 = at_Boss_Hatter_SpawnRoadHazard;
    dk::jump(at_Boss_Hatter_RoadCmdSpawnInLane);
}
