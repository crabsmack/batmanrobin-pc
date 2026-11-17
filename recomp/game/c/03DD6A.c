#include "game.h"

/* Boss_Hatter_RoadCmdSpawn3 - road schedule spawn kind 3: picks Boss_Hatter_SpawnRoadDriftRight, then
 * the spawn commands' common tail (lane, wait, spawn)
 * In:  a0/d4/d1 as Boss_Hatter_RoadCmdTbl; a4 = the boss's VM block
 * Out: a1 = the spawner; then Boss_Hatter_RoadCmdSpawnInLane's
 * Evidence: lea Boss_Hatter_SpawnRoadDriftRight,a1 / bra.s Boss_Hatter_RoadCmdSpawnInLane,
 *           Boss_Hatter_RoadCmdSpawn0's shape; the bra.s of Boss_Hatter_RoadCmdTbl entry 11; the road
 *           schedules the Hatter's scripts start (StartCoroutine2Rel targets $03F58C-$03FAB0 from
 *           $03E11A-$03F25A) use it; the corpus ran commands 0, 2 and 8 only; no corpus run reaches it
 *           (raw-stubs 2026-10-30); the contract is Boss_Hatter_RoadCmdSpawn0's (the same entry kind);
 *           03DD6A.syn checks it
 */
void Boss_Hatter_RoadCmdSpawn3(uint32_t *d0, uint16_t *d1, uint32_t *d4, uint16_t *d5, uint32_t *d7, uint32_t a0, uint32_t *a1, uint32_t a4, uint32_t a6)
{
    *a1 = at_Boss_Hatter_SpawnRoadDriftRight;
    dk::jump(at_Boss_Hatter_RoadCmdSpawnInLane);
}
