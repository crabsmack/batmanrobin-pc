#include "game.h"

/* Boss_Hatter_RoadCmdTbl_Spawn0 - entry 8 of Boss_Hatter_RoadCmdTbl: spawn kind 0 (the road hazard)
 * In:  as Boss_Hatter_RoadCmdSpawn0
 * Out: as Boss_Hatter_RoadCmdSpawn0
 * Evidence: bra.s Boss_Hatter_RoadCmdSpawn0 at $03DD42 ($03DD32 + 8 x 2); entries 9-15 (kinds 1-7)
 *           follow, raw and never run; TAS f114753/f114833
 */
void Boss_Hatter_RoadCmdTbl_Spawn0(uint32_t *d0, uint16_t *d1, uint32_t *d4, uint16_t *d5, uint32_t *d7, uint32_t a0,
                                   uint32_t a4, uint32_t a6)
{
    dk::jump(at_Boss_Hatter_RoadCmdSpawn0);
}
