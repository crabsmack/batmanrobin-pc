#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_03A81E = 0x03A81Eu;

/* Enemy_PropToy_SpawnChainWaypoints - spawner of a propeller toy (weight 5) with a shedding
 * 10-figure chain (ToyChain_MakeHanging10B) that flies a fixed course: points (x $A0, y $14),
 * ($104, $14), ($104, $50) $F00 ticks apart, then (-$64, $14) off to the left
 * In:  d4/d5 = spawn position (spawn entry)
 * Out: Obj_SpawnEnemy's result
 * Evidence: lea Scr2_03A81E / jmp Obj_SpawnEnemy; `244A 002A 0003A894`, goto $037E26, four `2D08 y
 *           x` gosubs (2-argument entry of Op2_GosubArgs: Arg1 = y, Arg0 = x) to $037EC2
 *           (Op2_AimAtArgPoint), timers $F00, then $B400 and gosub $037F0A; spawn entries
 *           Tbl_034142+$246/+$2D8; TAS f104480-f104884 (3 spawns, section $3300); png shot f104520:
 *           a figure chain hanging from the top right
 */
void Enemy_PropToy_SpawnChainWaypoints(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_03A81E;
    dk::jump(at_Obj_SpawnEnemy);
}
