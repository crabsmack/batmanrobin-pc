#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_039BAC = 0x039BACu;

/* ToyChain_SpawnDropIn9 - spawner of a drop-in toy chain of 9 figures (ToyChain_MakeDropIn9) on the
 * same falling base Scr2_037B12; population weight 1 (Op2_IncPopulation)
 * In:  d4/d5 = spawn position
 * Out: Obj_SpawnEnemy's result
 * Evidence: lea Scr2_039BAC / jmp Obj_SpawnEnemy; Scr2_039BAC `244A 002A 00039BE2`, `0A26`, goto
 *           Scr2_037B12; wave entries Scr5_034932+$86A; TAS f93331-f93388 (section $3200). A third
 *           such spawner at $039B50 (12 links via $039BC6, energy $20, weight $A) is never run,
 *           still raw
 */
void ToyChain_SpawnDropIn9(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                           uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_039BAC;
    dk::jump(at_Obj_SpawnEnemy);
}
