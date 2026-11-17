#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_03926C = 0x03926Cu;

enum { TOYRING_BLOCKS = 5 };   /* pool blocks for the ring: the usual 4 (Obj_SpawnEnemy) + 1 */

/* ToyRing_SpawnRandomAlt - spawner (5 blocks) of a random-size toy ring (gosub $0392CC) like
 * ToyRing_SpawnRandom but with the hop coroutine $039508 and charge cycle $038074, drop action $25
 * In:  d4/d5 = position (trigger or wave entry)
 * Out: jumps to Obj_SpawnEnemyN with a2 = Scr2_03926C, d7 = 5
 * Evidence: Scr2_03926C: Op2_SetSpawnAction $25, Op2_Gosub $0392CC, coroutines 2/3/1 =
 *           $039508/$038074/$0395F4; 23 table entries (Tbl_033CBE, Tbl_034142, Scr5_034932); TAS
 *           f79584-f112848 (13 frames)
 */
void ToyRing_SpawnRandomAlt(uint16_t *d1, uint16_t x, uint16_t y, uint16_t *blocks, uint32_t *a0,
                            uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_03926C;
    *blocks = TOYRING_BLOCKS;
    dk::jump(at_Obj_SpawnEnemyN);
}
