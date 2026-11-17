#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_03987E = 0x03987Eu;

/* ToyChain_Spawn7B - spawner of a toy-chain base with a 7-link chain (child $039986), charge cycle
 * $0380D8, shedding schedule $03870C
 * In:  d4/d5 = position (trigger entry)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_03987E
 * Evidence: Op2_SpawnChild $2A dc.l $039986 (dc.w 7); Tbl_034142 (+$4E4, +$4EC); in the corpus by
 *           ctarget
 */
void ToyChain_Spawn7B(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                      uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_03987E;
    dk::jump(at_Obj_SpawnEnemy);
}
