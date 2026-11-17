#include "game.h"

/* Boss_Hatter_RoadCmdTbl_Wait - entry 1 of Boss_Hatter_RoadCmdTbl: command 1 (wait byte 1 checker rows,
 * resuming after the command), a bra.s to Boss_Hatter_RoadCmdWait
 * In:  as Boss_Hatter_RoadCmdWait
 * Out: as Boss_Hatter_RoadCmdWait
 * Evidence: bra.s at $03DD34 = $03DD32 + 1 x 2; Boss_Hatter_RoadCmdNext's `jmp
 *           Boss_Hatter_RoadCmdTbl(pc,d0.w)` with d0 = (byte 0 & $F) x 2, so all 16 entries are
 *           reachable; the road schedules the Hatter's scripts start (StartCoroutine2Rel targets
 *           $03F58C-$03FAB0 from $03E11A-$03F25A) use it; the corpus ran commands 0, 2 and 8 only; no
 *           corpus run reaches it (raw-stubs 2026-10-30); the contract is Boss_Hatter_RoadCmdTbl_Goto's
 *           (the same entry kind); 03DD34.syn checks it
 */
void Boss_Hatter_RoadCmdTbl_Wait(uint32_t *d0, uint8_t *d1, uint32_t *d4, uint16_t *d5, uint32_t a0, uint32_t a4, uint32_t a6)
{
    dk::jump(at_Boss_Hatter_RoadCmdWait);
}
