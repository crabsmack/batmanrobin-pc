#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_039B84 = 0x039B84u;

/* ToyChain_SpawnDropIn12 - spawner of a drop-in toy chain (script Scr2_039B84, body Scr2_037B12):
 * an invisible base that falls from above the screen onto the terrain, kicks up dust on landing and
 * holds a 12-figure chain (ToyChain_MakeDropIn12); population weight 5
 * In:  d4/d5 = spawn position (from the wave entry)
 * Out: Obj_SpawnEnemy's result
 * Evidence: lea Scr2_039B84 / jmp Obj_SpawnEnemy; Scr2_039B84 `244A 002A 00039BD4`, `0A32 0005`
 *           (Op2_AddPopulation), goto Scr2_037B12 (op $1CDC 0028 0180 = fall; on the floor
 *           Op2_FxSpawnOffAbs Scr4_010D9A/Scr4_010DAC + sound $B); wave entries Scr5_034932+$83E;
 *           TAS f92263-f92886 (7 spawns, section $3200 mushroom forest); png shot f92290: a figure
 *           column coming down from the top, f92330: standing on the floor
 */
void ToyChain_SpawnDropIn12(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                            uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_039B84;
    dk::jump(at_Obj_SpawnEnemy);
}
