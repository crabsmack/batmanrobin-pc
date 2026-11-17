#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_0396CE = 0x0396CEu;

/* ToyChain_Spawn10B - spawner of a toy-chain base carrying a 10-link chain (child $039A06) with
 * charge cycle $038578 and shedding schedule $038796
 * In:  d4/d5 = position (wave entry)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_0396CE
 * Evidence: Scr2_0396CE: Op2_SpawnChild $2A dc.l $039A06; wave script Scr5_034932+$2A4; TAS f86799
 *           (1 frame)
 */
void ToyChain_Spawn10B(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                       uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_0396CE;
    dk::jump(at_Obj_SpawnEnemy);
}
