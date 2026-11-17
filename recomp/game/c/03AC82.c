#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_03AC8E = 0x03AC8Eu;

/* Enemy_PropToy_SpawnChain5RightB - spawner of a propeller toy entering from the right (H-flipped)
 * with the 5-link chain ToyChain_MakeHanging5B (charge cycle $038810, no coroutine 0), course
 * $03AD7C, body $037E26
 * In:  d4/d5 = position (wave spawn entry: x $168, screen y $10)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_03AC8E
 * Evidence: Scr2_03AC8E: Op2_SpawnChild $2A dc.l $03AFD8, Op2_ModSprAttr $F008; entries in
 *           Scr5_034932+$994 and Scr5_0352CA (+$1C6, +$20A, +$27E); TAS f94297-f95872 (5 spawns)
 */
void Enemy_PropToy_SpawnChain5RightB(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_03AC8E;
    dk::jump(at_Obj_SpawnEnemy);
}
