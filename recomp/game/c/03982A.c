#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_039836 = 0x039836u;

/* ToyChain_Spawn5B - spawner of a toy-chain base with a 5-link chain (child $039942) using the
 * second charge cycle $0380D8 and the fixed-delay shedding schedule $03870C
 * In:  d4/d5 = position (trigger entry)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_039836
 * Evidence: Op2_SpawnChild $2A dc.l $039942 (dc.w 5); Tbl_034142 (+$4C4, +$4CC); TAS f110618 (1
 *           frame)
 */
void ToyChain_Spawn5B(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                      uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_039836;
    dk::jump(at_Obj_SpawnEnemy);
}
