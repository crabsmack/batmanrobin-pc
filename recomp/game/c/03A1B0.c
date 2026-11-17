#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_03A1BC = 0x03A1BCu;

/* Enemy_GiftBox_SpawnStay8B - second copy of Enemy_GiftBox_SpawnStay8A (Scr2_03A1BC has the same
 * ops and the same chain $03A456, only its own address), used by the next wave entry
 * In:  d4/d5 = spawn position (wave entry)
 * Out: Obj_SpawnEnemy's result
 * Evidence: lea Scr2_03A1BC / jmp Obj_SpawnEnemy; script `0A32 000A`, `244A 002A 0003A456`, timer
 *           $4B0; wave entry Scr5_034932+$272; TAS f83732 (section $3100)
 */
void Enemy_GiftBox_SpawnStay8B(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                               uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_03A1BC;
    dk::jump(at_Obj_SpawnEnemy);
}
