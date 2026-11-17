#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_03AA26 = 0x03AA26u;

/* Enemy_PropToy_SpawnChain8Left - spawner of a propeller toy entering from the left (not flipped)
 * with the 8-link chain ToyChain_MakeHanging8, course coroutine $03AA3A, body $037E26 (population
 * weight 5)
 * In:  d4/d5 = position (wave spawn entry: x -$28, screen y $10)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_03AA26
 * Evidence: Scr2_03AA26: Op2_SpawnChild $2A dc.l $03AEEA, no Op2_ModSprAttr; entry `FFD8 E010
 *           2903AA1A` (spawn action $29) at Scr2_03554C+$3C; TAS f97264
 */
void Enemy_PropToy_SpawnChain8Left(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_03AA26;
    dk::jump(at_Obj_SpawnEnemy);
}
