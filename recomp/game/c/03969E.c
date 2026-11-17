#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_0396AA = 0x0396AAu;

/* ToyChain_Spawn10A - spawner of a toy-chain base carrying a 10-link chain (child $039A06, table
 * $039AFC) with charge cycle $038596 and shedding schedule $0387A4
 * In:  d4/d5 = position (trigger or wave entry)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_0396AA (Op2_SpawnChild, child coroutines 3/0, then
 *      the base loop Scr2_0379BC)
 * Evidence: Scr2_0396AA: Op2_SpawnChild $2A dc.l $039A06 (ToyChain_SpawnInline, dc.w $A);
 *           Tbl_034142 (+$504, +$50C), Scr5_034932+$29C; TAS f86799 (1 frame)
 */
void ToyChain_Spawn10A(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                       uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_0396AA;
    dk::jump(at_Obj_SpawnEnemy);
}
