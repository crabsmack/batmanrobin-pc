#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_03ACCA = 0x03ACCAu;

/* Enemy_PropToy_SpawnChain5LeftB - spawner of a propeller toy entering from the left with the
 * 5-link chain ToyChain_MakeHanging5D (charge cycle $038810), course $03AD3E, body $037E26
 * In:  d4/d5 = position (wave spawn entry: x -$28, screen y $10)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_03ACCA
 * Evidence: Scr2_03ACCA: Op2_SpawnChild $2A dc.l $03B034; entries in Scr5_034932+$984 and
 *           Scr5_0352CA (+$1B6, +$1FA, +$26E); TAS f93781-f95691 (9 spawns); shot f93825: the
 *           propeller toy with figures hanging below at the top left
 */
void Enemy_PropToy_SpawnChain5LeftB(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_03ACCA;
    dk::jump(at_Obj_SpawnEnemy);
}
