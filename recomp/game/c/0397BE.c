#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_0397CA = 0x0397CAu;

/* ToyChain_Spawn6 - spawner of a toy-chain base with a 6-link chain (ToyChain_MakeChild6), charge
 * cycle $0380B8 and shedding schedule Scr2_0386CC
 * In:  d4/d5 = position (wave entry)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_0397CA
 * Evidence: Op2_SpawnChild $2A dc.l ToyChain_MakeChild6; Scr5_034932 (+$242, +$24A); TAS
 *           f84217-f85120 (4 frames)
 */
void ToyChain_Spawn6(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                     uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_0397CA;
    dk::jump(at_Obj_SpawnEnemy);
}
