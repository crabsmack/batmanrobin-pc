#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_03AC6E = 0x03AC6Eu;

/* Enemy_PropToy_SpawnChain5Right - spawner of a propeller toy entering from the right (H-flipped)
 * with the 5-link chain ToyChain_MakeHanging5A, course $03AD7C (cruise x $200, waits, slows,
 * leaves), body $037E26
 * In:  d4/d5 = position (wave spawn entry: x $168, screen y $10)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_03AC6E
 * Evidence: Scr2_03AC6E: Op2_SpawnChild $2A dc.l $03AFA8, Op2_ModSprAttr $F008, Op2_Goto $03AD7C;
 *           entries in Scr5_034932+$98C (action $29) and Scr5_0352CA (+$1BE, +$202, +$276); TAS
 *           f94419-f95660 (4 spawns)
 */
void Enemy_PropToy_SpawnChain5Right(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_03AC6E;
    dk::jump(at_Obj_SpawnEnemy);
}
