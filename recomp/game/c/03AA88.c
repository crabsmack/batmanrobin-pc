#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_03AA94 = 0x03AA94u;

/* Enemy_PropToy_SpawnChain6Right - spawner of a propeller toy entering from the right (H-flipped)
 * with the 6-link chain ToyChain_MakeHanging6B, course coroutine $03AAAC (cruise $1AA, two waypoint
 * legs), body $037E26
 * In:  d4/d5 = position (trigger entry: x $168, screen y $10)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_03AA94
 * Evidence: Scr2_03AA94: Op2_SpawnChild $2A dc.l $03AF1A, Op2_ModSprAttr $F008; dc.l $03AA88 in
 *           Tbl_034142+$4FC; ctarget in 1 corpus run, not in the TAS
 */
void Enemy_PropToy_SpawnChain6Right(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_03AA94;
    dk::jump(at_Obj_SpawnEnemy);
}
