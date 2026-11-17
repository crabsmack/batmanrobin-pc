#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_03A022 = 0x03A022u;

/* Enemy_GiftBox_SpawnRoam4A - spawner of a gift box (weight $A) that opens into a 4-figure swaying
 * chain (ToyChain_MakeBox4) and roams at cruise x $2AA with low hops ($28/$F0) every $4B0 ticks
 * In:  d4/d5 = spawn position (wave entry)
 * Out: Obj_SpawnEnemy's result
 * Evidence: lea Scr2_03A022 / jmp Obj_SpawnEnemy; script `0A32 000A`, `244A 002A 0003A398`, `17B8
 *           02AA`, `1C8C 0028 00F0`, timer $4B0; wave entries Scr5_034932+$55A/+$56A/+$640/+$69C;
 *           TAS f82947 (section $3100)
 */
void Enemy_GiftBox_SpawnRoam4A(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                               uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_03A022;
    dk::jump(at_Obj_SpawnEnemy);
}
