#include "game.h"

/* the road object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_03FDD2 = 0x03FDD2u;

/* Boss_Hatter_SpawnRoadDriftLeft - road schedule spawn kind 4: a road object (body $03DE16) that drifts
 * left as it comes (x accel $19D/$60, cruise -$500)
 * In:  d4 = the lane's road x, d5 = y (replaced by the road distance); a4 = the boss's VM block
 * Out: a6 = the object, d0 = 0 (the placement); a full pool quits the caller (Obj_SpawnEnemyOrQuit
 *      returns past this)
 * Evidence: lea Scr2_03FDD2,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Stage3_RoadPlaceAtHorizon; named by the
 *           lea at $03DD72 (Boss_Hatter_RoadCmdSpawn4, entry 12 of Boss_Hatter_RoadCmdTbl); the
 *           Hatter's road schedules (StartCoroutine2Rel targets $03F58C-$03FAB0 from the boss scripts
 *           $03E11A-$03F25A) use these kinds; the corpus ran kind 0 only; no corpus run reaches it
 *           (raw-stubs 2026-10-30); the contract and the C are Boss_Hatter_SpawnRoadHazard's (the same
 *           jsr (a1)); 03FDC2.syn checks it
 */
void Boss_Hatter_SpawnRoadDriftLeft(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a4,
                                    uint32_t *a6)
{
    uint32_t list = 0, motion = 0, sprite = 0;
    Obj_SpawnEnemyOrQuit(d0, d1, d4, d5, d7, &list, Scr2_03FDD2, &motion, &a4, &sprite, a6);
    dk::jump(at_Stage3_RoadPlaceAtHorizon);
}
