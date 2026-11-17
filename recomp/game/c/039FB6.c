#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_039FC2 = 0x039FC2u;

/* Enemy_GiftBox_SpawnStay10B - spawner of a gift box (weight 5) that stays put, opens into a
 * 10-figure swaying chain (ToyChain_MakeBox10B) and hops in place every $780 ticks (differs from
 * SpawnStay10A only in its chain)
 * In:  d4/d5 = spawn position (wave entry)
 * Out: Obj_SpawnEnemy's result
 * Evidence: lea Scr2_039FC2 / jmp Obj_SpawnEnemy; script `0A32 0005`, `244A 002A 0003A300`, `17B8
 *           0000`, timer $780 then hop; wave entries Scr5_034932+$9A/+$164; TAS f79868-f91266 (3
 *           spawns, sections $3100-$3200)
 */
void Enemy_GiftBox_SpawnStay10B(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                                uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_039FC2;
    dk::jump(at_Obj_SpawnEnemy);
}
