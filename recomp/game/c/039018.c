#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_039028 = 0x039028u;

enum { TOYRING_BLOCKS = 5 };   /* pool blocks for the ring: the usual 4 (Obj_SpawnEnemy) + 1 */

/* ToyRing_SpawnBigB - spawner (5 blocks, Obj_SpawnEnemyN) of a 16-figure toy ring (radius $6E,
 * stiffness $40, as ToyRing_SpawnBig) with energy $200 and 300 points, its own coroutines
 * In:  d4/d5 = position (wave spawn entry)
 * Out: jumps to Obj_SpawnEnemyN with a2 = Scr2_039028, d7 = 5
 * Evidence: ToyRing_SpawnBig's stub; the wave spawn entry at $03491E (x $14A, y $E070) names it,
 *           through VM5_SpawnEntry's jsr (a0) $007016; ToyRing_Init words $10 $6E $40 3 $CCCC,
 *           Op2_SetEnergy $200, Op2_SetScoreValue $12C. No corpus run calls it; 039018.syn checks it.
 */
void ToyRing_SpawnBigB(uint16_t *d1, uint16_t x, uint16_t y, uint16_t *blocks, uint32_t *script, uint32_t *a3,
                       uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_039028;
    *blocks = TOYRING_BLOCKS;
    dk::jump(at_Obj_SpawnEnemyN);
}
