#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_039812 = 0x039812u;

/* ToyChain_Spawn8 - spawner of a toy-chain base with an 8-link chain (child $039920), charge cycle
 * $0380B8 and shedding schedule Scr2_0386CC
 * In:  d4/d5 = position (wave entry)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_039812
 * Evidence: Op2_SpawnChild $2A dc.l $039920 (dc.w 8); Scr5_034932+$262; TAS f86023 (1 frame)
 */
void ToyChain_Spawn8(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                     uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_039812;
    dk::jump(at_Obj_SpawnEnemy);
}
