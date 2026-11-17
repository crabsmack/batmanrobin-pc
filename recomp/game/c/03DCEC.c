#include "game.h"

/* Boss_Hatter_RoadCmdLoop - road schedule command 6: counts the loop count down; while it stays above
 * 0 the command is a goto (the 12-bit offset of bytes 0/1), else the next command is read
 * In:  a0 = past the command, d4 = byte 0, d1 = byte 1; a4 = the boss's VM block
 * Out: hatter_road_scene()->loop - 1; then Boss_Hatter_RoadCmdGoto (still above 0, signed) or
 *      Boss_Hatter_RoadCmdNext
 * Evidence: subq.b #1,(g_SceneVars+$6).w / bgt.s Boss_Hatter_RoadCmdGoto / bra.s Boss_Hatter_RoadCmdNext;
 *           reached by Boss_Hatter_RoadCmdTbl entry 6 ($03DD3E); no corpus run reaches it (raw-stubs
 *           2026-10-30); 03DCEC.syn checks it
 */
void Boss_Hatter_RoadCmdLoop(uint32_t *d0, uint8_t *d1, uint32_t *d4, uint16_t *d5, uint32_t *a0, uint32_t a4,
                             uint32_t a6)
{
    HatterRoadScene *scene = hatter_road_scene();
    uint8_t count = scene->loop;
    scene->loop = (uint8_t)(count - 1);
    /* bgt after subq.b: Z clear and N == V of count - 1 (V when count was $80) */
    bool greater = (int8_t)count > 1;
    if (greater) {
        dk::jump(at_Boss_Hatter_RoadCmdGoto);
        return;
    }
    dk::jump(at_Boss_Hatter_RoadCmdNext);
}
