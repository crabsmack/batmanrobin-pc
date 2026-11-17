#include "game.h"

/* Boss_Hatter_RoadCmdWait - road schedule command 1: waits byte 1 checker rows and stores the
 * resume pointer (past the command) in the boss's slot 2; the schedule step ends here
 * In:  a0 = past the command, d1 = byte 1 (the rows to wait), a4 = the boss's VM block
 * Out: hatter_road_scene()->wait = d1; the boss's slot 2 ($40(a4)) = a0; rts
 * Evidence: move.b d1,(g_SceneVars+$10).w / move.l a0,$40(a4) / rts (the store Boss_Hatter_RoadCmdSpawnInLane
 *           also makes); reached by Boss_Hatter_RoadCmdTbl entry 1 ($03DD34); the Hatter's road schedules
 *           use it (e.g. $03F60C); no corpus run reaches it (raw-stubs 2026-10-30); 03DCDC.syn checks it
 */
void Boss_Hatter_RoadCmdWait(uint32_t *d0, uint8_t *d1, uint32_t *d4, uint16_t *d5, uint32_t *a0, uint32_t a4,
                             uint32_t a6)
{
    hatter_road_scene()->wait = *d1;
    PTR(Vm2Block) boss_vm;
    boss_vm.v = a4;
    boss_vm->coroutine[2].v = *a0;
}
