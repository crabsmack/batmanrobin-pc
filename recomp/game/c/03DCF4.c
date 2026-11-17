#include "game.h"

/* Boss_Hatter_RoadCmdGosub - road schedule command 3: pushes the pointer past the command on the
 * schedule's gosub stack (4 bytes a level) and falls into the goto (the 12-bit offset of bytes 0/1)
 * In:  a0 = past the command, d4 = byte 0, d1 = byte 1; a4 = the boss's VM block
 * Out: the stack slot at the old depth = a0, depth + 4, a1 = the slot; then Boss_Hatter_RoadCmdGoto
 * Evidence: lea (g_SceneVars+$8).w,a1 / adda.w (g_SceneVars+$4).w,a1 / addq.w #4,(g_SceneVars+$4).w /
 *           move.l a0,(a1), then the next file Boss_Hatter_RoadCmdGoto; reached by
 *           Boss_Hatter_RoadCmdTbl entry 3 ($03DD38); no corpus run reaches it (raw-stubs 2026-10-30);
 *           03DCF4.syn checks it. The slot is the stack's base + the depth in bytes, as the asm (two
 *           levels fit in the stack; a deeper one would write past it, as on the 68000)
 */
void Boss_Hatter_RoadCmdGosub(uint32_t *d0, uint8_t *d1, uint32_t *d4, uint16_t *d5, uint32_t *a0, uint32_t *a1,
                              uint32_t a4, uint32_t a6)
{
    HatterRoadScene *scene = hatter_road_scene();
    uint16_t depth = scene->depth;
    PTR(u32) slot;
    slot.v = ADDR68(&scene->stack[0]) + (uint32_t)(int32_t)(int16_t)depth;
    *a1 = slot.v;
    scene->depth = (uint16_t)(depth + 4);
    *slot = *a0;
    dk::jump(at_Boss_Hatter_RoadCmdGoto);
}
