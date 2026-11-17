#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_03905E = 0x03905Eu;

enum { TOYRING_BLOCKS = 5 };   /* pool blocks for the ring: the usual 4 (Obj_SpawnEnemy) + 1 */

/* ToyRing_SpawnBig - spawner (5 blocks, Obj_SpawnEnemyN) of the big toy ring: 16 figures, radius
 * $6E, energy $400, 1000 points (Scr2_03905E), shared ring body Scr2_037588
 * In:  d4/d5 = position (trigger entry)
 * Out: jumps to Obj_SpawnEnemyN with a2 = Scr2_03905E, d7 = 5
 * Evidence: ToyRing_Init words $10 $6E $40 3 $CCCC; Op2_SetEnergy $400, Op2_SetScoreValue $3E8;
 *           Tbl_034142 (+$3C0, +$42E); TAS f109594 (1 frame)
 */
void ToyRing_SpawnBig(uint16_t *d1, uint16_t x, uint16_t y, uint16_t *blocks, uint32_t *script, uint32_t *a3,
                      uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_03905E;
    *blocks = TOYRING_BLOCKS;
    dk::jump(at_Obj_SpawnEnemyN);
}
