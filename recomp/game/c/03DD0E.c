#include "game.h"

/* Boss_Hatter_RoadCmdReturn - road schedule command 4: pops the schedule's gosub stack (depth - 4)
 * into the schedule pointer and reads the next command there
 * In:  a4 = the boss's VM block (the command's bytes are not used)
 * Out: depth - 4, a0 = the popped pointer; then Boss_Hatter_RoadCmdNext
 * Evidence: subq.w #4,(g_SceneVars+$4).w / lea (g_SceneVars+$8).w,a0 / adda.w (g_SceneVars+$4).w,a0 /
 *           movea.l (a0),a0 / bra.s Boss_Hatter_RoadCmdNext; reached by Boss_Hatter_RoadCmdTbl entry 4
 *           ($03DD3A); no corpus run reaches it (raw-stubs 2026-10-30); 03DD0E.syn checks it
 */
void Boss_Hatter_RoadCmdReturn(uint32_t *d0, uint8_t *d1, uint32_t *d4, uint16_t *d5, uint32_t *a0, uint32_t a4,
                               uint32_t a6)
{
    HatterRoadScene *scene = hatter_road_scene();
    scene->depth = (uint16_t)(scene->depth - 4);
    PTR(const u32) slot;
    slot.v = ADDR68(&scene->stack[0]) + (uint32_t)(int32_t)(int16_t)scene->depth;
    *a0 = *slot;
    dk::jump(at_Boss_Hatter_RoadCmdNext);
}
