#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_0391F4 = 0x0391F4u;

enum { TOYRING_BLOCKS = 5 };   /* pool blocks for the ring: the usual 4 (Obj_SpawnEnemy) + 1 */

/* ToyRing_SpawnSix - spawner (5 blocks) of a six-figure toy ring (radius $32, stiffness $60),
 * energy $C0, drop action $25, splitting into single figures later (coroutine 1 $039612)
 * In:  d4/d5 = position (trigger entry)
 * Out: jumps to Obj_SpawnEnemyN with a2 = Scr2_0391F4, d7 = 5
 * Evidence: ToyRing_Init words 6 $32 $60 1 $9248; Tbl_034142 (+$3B0, +$41E, +$754); TAS
 *           f109008-f112758 (4 frames)
 */
void ToyRing_SpawnSix(uint16_t *d1, uint16_t x, uint16_t y, uint16_t *blocks, uint32_t *script, uint32_t *a3,
                      uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_0391F4;
    *blocks = TOYRING_BLOCKS;
    dk::jump(at_Obj_SpawnEnemyN);
}
