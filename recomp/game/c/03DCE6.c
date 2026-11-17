#include "game.h"

/* Boss_Hatter_RoadCmdSetLoop - road schedule command 5: sets the schedule's loop count to byte 1 and
 * reads the next command
 * In:  a0 = past the command, d1 = byte 1 (the count), d4 = byte 0; a4 = the boss's VM block
 * Out: hatter_road_scene()->loop = d1; then Boss_Hatter_RoadCmdNext
 * Evidence: move.b d1,(g_SceneVars+$6).w / bra.s Boss_Hatter_RoadCmdNext; reached by
 *           Boss_Hatter_RoadCmdTbl entry 5 ($03DD3C); the Hatter's road schedules use it (`0503` at
 *           $03F60C); no corpus run reaches it (raw-stubs 2026-10-30); 03DCE6.syn checks it
 */
void Boss_Hatter_RoadCmdSetLoop(uint32_t *d0, uint8_t *d1, uint32_t *d4, uint16_t *d5, uint32_t *a0, uint32_t a4,
                                uint32_t a6)
{
    hatter_road_scene()->loop = *d1;
    dk::jump(at_Boss_Hatter_RoadCmdNext);
}
