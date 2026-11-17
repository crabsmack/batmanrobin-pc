#include "game.h"

/* Boss_Hatter_RoadCmdTbl_Spawn3 - entry 11 of Boss_Hatter_RoadCmdTbl: spawn kind 3
 * (Boss_Hatter_SpawnRoadDriftRight), a bra.s to Boss_Hatter_RoadCmdSpawn3
 * In:  as Boss_Hatter_RoadCmdSpawn3
 * Out: as Boss_Hatter_RoadCmdSpawn3
 * Evidence: bra.s at $03DD48 = $03DD32 + 11 x 2; Boss_Hatter_RoadCmdNext's `jmp
 *           Boss_Hatter_RoadCmdTbl(pc,d0.w)` with d0 = (byte 0 & $F) x 2, so all 16 entries are
 *           reachable; the road schedules the Hatter's scripts start (StartCoroutine2Rel targets
 *           $03F58C-$03FAB0 from $03E11A-$03F25A) use it; the corpus ran commands 0, 2 and 8 only; no
 *           corpus run reaches it (raw-stubs 2026-10-30); the contract is
 *           Boss_Hatter_RoadCmdTbl_Spawn0's (the same entry kind); 03DD48.syn checks it
 */
void Boss_Hatter_RoadCmdTbl_Spawn3(uint32_t *d0, uint16_t *d1, uint32_t *d4, uint16_t *d5, uint32_t *d7, uint32_t a0, uint32_t a4, uint32_t a6)
{
    dk::jump(at_Boss_Hatter_RoadCmdSpawn3);
}
