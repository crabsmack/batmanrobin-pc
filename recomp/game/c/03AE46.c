#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_03AE52 = 0x03AE52u;

/* Enemy_PropToy_SpawnTwinChain10 - spawner of a propeller toy entering from the right (H-flipped)
 * with two 10-link chains (children $2A ToyChain_MakeHanging10C, $2C ToyChain_MakeHanging10D, anims
 * $037F1E/$037F4A), cruise $155, long waits; body $037E26
 * In:  d4/d5 = position (wave spawn entry: x $168, screen y $10)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_03AE52
 * Evidence: Scr2_03AE52: two Op2_SpawnChild ops (dc.l $03B0C0, $03B0EE), Op2_ModSprAttr $F008;
 *           entry `0168 E010 1C03AE46` (spawn action $1C, an extra-life pickup) at Scr2_03554C+$44;
 *           TAS f97692
 */
void Enemy_PropToy_SpawnTwinChain10(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_03AE52;
    dk::jump(at_Obj_SpawnEnemy);
}
