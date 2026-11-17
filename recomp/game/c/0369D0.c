#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_0369DC = 0x0369DCu;

/* Enemy_DropCarrier_SpawnFigures - spawner of a drop carrier that crosses at screen height $84 and
 * drops toy figures (ToyFigure_Spawn) in bursts of four (Scr2_0369DC: population weight $A)
 * In:  x/y = spawn position (d4/d5)
 * Out: script = Scr2_0369DC, then Obj_SpawnEnemy's (d0 = 0 and the new object's blocks;
 *      d0 nonzero when the pool is short)
 * Evidence: lea Scr2_0369DC,a2 / jmp Obj_SpawnEnemy; Tbl_034142 (+$5C0, +$656); TAS f110781
 */
void Enemy_DropCarrier_SpawnFigures(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_0369DC;
    dk::jump(at_Obj_SpawnEnemy);
}
