#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_03FCB0 = 0x03FCB0u;

/* Boss_Hatter_SpawnRoadPieceRightSlow1 - one of the three pieces a Boss_Hatter_SpawnRoadSplitRightSoon
 * road object bursts into: flies right (x accel $19D, cruise $500) on the piece body $03DE04, linked
 * right after its parent (Obj_SpawnEnemyAfter)
 * In:  d4/d5 = the parent's position (Spawn_AtOwnPos, Op2_CallNative $008D66 in the parent's coroutine
 *      0)
 * Out: script (a2) = Scr2_03FCB0, then Obj_SpawnEnemyAfter's results
 * Evidence: lea Scr2_03FCB0,a2 / jmp Obj_SpawnEnemyAfter; lea script / jmp Obj_SpawnEnemyAfter; script:
 *           SetSpeed $100, SetAccelXForDist $19D $30/$60, SetCruiseX $500, Goto $03DE04; dc.l of
 *           Op2_CallNative Spawn_AtOwnPos at $03FC88 in Boss_Hatter_SpawnRoadSplitRightSoon's coroutine
 *           0; no corpus run reaches it (raw-stubs 2026-10-30); the contract is
 *           Enemy_SpinMine_SpawnDown's (the same kind of call); 03FCA4.syn checks it
 */
void Boss_Hatter_SpawnRoadPieceRightSlow1(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t a4, uint32_t a6)
{
    *script = Scr2_03FCB0;
    dk::jump(at_Obj_SpawnEnemyAfter);
}
