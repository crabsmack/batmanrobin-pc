#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_039186 = 0x039186u;

enum { TOYRING_BLOCKS = 5 };   /* pool blocks for the ring: the usual 4 (Obj_SpawnEnemy) + 1 */

/* ToyRing_SpawnRandom - spawner (5 blocks) of a toy ring of random size (gosub $0392CC: 4-7
 * figures, radius $1E-$3C), energy $C0, drop action $25, which later splits into single figures
 * (coroutine 1 $0395F4)
 * In:  d4/d5 = position (wave spawn entry)
 * Out: jumps to Obj_SpawnEnemyN with a2 = Scr2_039186, d7 = 5
 * Evidence: Op2_Gosub $0392CC (Op2_SwitchRandom over four ToyRing_Init calls); Op2_SetSpawnAction
 *           $25; wave script Scr5_034932 (7 entries); TAS f80988-f83339 (5 frames)
 */
void ToyRing_SpawnRandom(uint16_t *d1, uint16_t x, uint16_t y, uint16_t *blocks, uint32_t *script,
                         uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_039186;
    *blocks = TOYRING_BLOCKS;
    dk::jump(at_Obj_SpawnEnemyN);
}
