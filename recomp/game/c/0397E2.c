#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_0397EE = 0x0397EEu;

/* ToyChain_Spawn7 - spawner of a toy-chain base with a 7-link chain (child $0398FE), charge cycle
 * $0380B8 and shedding schedule Scr2_0386CC
 * In:  d4/d5 = position (wave entry)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_0397EE
 * Evidence: Op2_SpawnChild $2A dc.l $0398FE (dc.w 7); Scr5_034932 (+$252, +$25A); TAS f85421 (1
 *           frame)
 */
void ToyChain_Spawn7(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                     uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_0397EE;
    dk::jump(at_Obj_SpawnEnemy);
}
