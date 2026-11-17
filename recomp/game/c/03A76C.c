#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_03A778 = 0x03A778u;

/* Enemy_PropToy_SpawnChain10 - spawner of a propeller toy (weight 5) with a hanging 10-figure chain
 * (ToyChain_MakeHanging10): holds a player window (cruise $100/$100) for $2580 ticks, then flies
 * off toward x -$64
 * In:  d4/d5 = spawn position (spawn entry with action $11)
 * Out: Obj_SpawnEnemy's result
 * Evidence: lea Scr2_03A778 / jmp Obj_SpawnEnemy; `244A 002A 0003A7C8`, goto $037E26, `2D00 FFD8
 *           FF9C 0000 0000` -> $037EE6, timer $2580, `2D08 0000 FF9C` -> $037EC2
 *           (Op2_AimAtArgPoint, Arg0 = x -$64); spawn entries Tbl_034142+$23E/+$2D0; not in the
 *           TAS: fuzz-104400-s1/s2 f106141-f107945 (section $3300)
 */
void Enemy_PropToy_SpawnChain10(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_03A778;
    dk::jump(at_Obj_SpawnEnemy);
}
