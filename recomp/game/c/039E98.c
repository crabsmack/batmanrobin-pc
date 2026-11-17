#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_039EA4 = 0x039EA4u;

/* Enemy_GiftBox_SpawnRoam1HopFast - as Enemy_GiftBox_SpawnRoam1 (weight 1, one figure, cruise x
 * $2AA) but hopping every $5A0 ticks
 * In:  d4/d5 = spawn position (spawn entry)
 * Out: Obj_SpawnEnemy's result
 * Evidence: lea Scr2_039EA4 / jmp Obj_SpawnEnemy; script `0A26`, `244A 002A 0003A2E6`, `17B8 02AA`,
 *           timer $5A0; spawn entries Tbl_034142+$312/+$734; TAS f108390-f112233 (section $3400).
 *           The weight-$A stay-put variant after it ($039EF6, Scr2_039F02, hop every $780) is never
 *           run, still raw
 */
void Enemy_GiftBox_SpawnRoam1HopFast(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                                     uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_039EA4;
    dk::jump(at_Obj_SpawnEnemy);
}
