#include "game.h"

/* Boss_Hatter_RoadCmdTbl - entry 0 of the road schedule's command jump table (16 bra.s entered by
 * Boss_Hatter_RoadCmdNext's `jmp (pc,d0.w)`): command 0, the end of the schedule
 * In:  a4 = the boss's VM block
 * Out: as Boss_Hatter_RoadCmdEnd
 * Evidence: bra.s loc_03DCCE (Boss_Hatter_RoadCmdEnd) at $03DD32; 3 corpus runs
 */
void Boss_Hatter_RoadCmdTbl(uint32_t a4)
{
    dk::jump(at_Boss_Hatter_RoadCmdEnd);
}
