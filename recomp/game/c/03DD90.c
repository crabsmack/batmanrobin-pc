#include "game.h"

enum {
    ROAD_LANES = 7,            /* lane 0 = a random one of the 7 */
    ROAD_LANE_WIDTH = 56,      /* road x of lane n = 56n - 28 */
    ROAD_LANE_HALF = 0x1C,
};

/* Boss_Hatter_RoadCmdSpawnInLane - common tail of the road schedule's spawn commands: stores the
 * wait (byte 1 rows) and the resume pointer, picks the road x of lane n = byte 0's high nibble
 * (56n - 28; lane 0 = a random one of the 7), calls the kind's spawner there, then goes back to
 * the wait countdown
 * In:  a1 = the spawner; a0 = past the command, d4's low byte = byte 0, d1 = byte 1; a4 = the
 *      boss's VM block; d5 passes through to the spawner
 * Out: wait, the boss's slot 2 = a0; d4's low word = the lane x; d0/d1/d7 as the spawner leaves
 *      them (random lane: d0 = Rand_Below's x 8 before the call); a0/a4 kept (movem); then
 *      Boss_Hatter_RoadSchedCountWait's
 * Evidence: move.b d1,(g_SceneVars+$10).w / move.l a0,$40(a4) / andi.w #$F0,d4 / bne; moveq #7,d0 /
 *           Rand_Below / asl.w #3 / d4 = d0 x 8 - d0 + $1C; else d4 x 4 - d4 / 2 - $1C; movem.l
 *           a0/a4 / jsr (a1) / movem.l / bra.w Boss_Hatter_RoadSchedCountWait; TAS f114753-f114833
 */
void Boss_Hatter_RoadCmdSpawnInLane(uint32_t *d0, uint16_t *d1, uint32_t *d4, uint16_t *d5, uint32_t *d7, uint32_t a0,
                                    uint32_t a1, uint32_t a4, uint32_t a6)
{
    hatter_road_scene()->wait = (uint8_t)*d1;
    PTR(Vm2Block) boss_vm;
    boss_vm.v = a4;
    boss_vm->coroutine[2].v = a0;
    uint16_t lane = (uint16_t)(*d4 & 0xF0);          /* the lane x 16 */
    uint16_t x;
    if (lane == 0) {
        *d0 = ROAD_LANES;
        Rand_Below(d0);
        uint16_t eighth = (uint16_t)(*d0 << 3);       /* asl.w #3: lane x 8 */
        *d0 = (*d0 & 0xFFFF0000u) | eighth;
        x = (uint16_t)(eighth * 7 + ROAD_LANE_HALF);
    } else {
        x =(uint16_t)(lane * 4 - (uint16_t)((int16_t)lane >> 1) - ROAD_LANE_HALF);
        *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)((int16_t)lane >> 1);
    }
    *d4 = (*d4 & 0xFFFF0000u) | x;
    uint32_t saved_a4 = a4, spawned = a6;              /* movem.l a0/a4: a0 and a4 come back */
    RoadSpawnerFn(a1, d0, d1, x, *d5, d7, saved_a4, &spawned);
    dk::jump(at_Boss_Hatter_RoadSchedCountWait);
}
