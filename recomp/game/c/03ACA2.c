#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_03ACAE = 0x03ACAEu;

/* Enemy_PropToy_SpawnChain5Left - spawner of a propeller toy entering from the left with the 5-link
 * chain ToyChain_MakeHanging5C, course $03AD3E (cruise x $200, waits, slows), body $037E26
 * In:  d4/d5 = position (wave spawn entry: x -$28, screen y $10)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_03ACAE
 * Evidence: Scr2_03ACAE: Op2_SpawnChild $2A dc.l $03B004, Op2_Goto $03AD3E; entries in
 *           Scr5_034932+$97C (action $29) and Scr5_0352CA (+$1AE, +$1F2, +$266); TAS f93540-f95903
 *           (6 spawns)
 */
void Enemy_PropToy_SpawnChain5Left(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_03ACAE;
    dk::jump(at_Obj_SpawnEnemy);
}
