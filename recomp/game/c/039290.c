#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_0392A0 = 0x0392A0u;

enum { TOYRING_BLOCKS = 5 };   /* pool blocks for the ring: the usual 4 (Obj_SpawnEnemy) + 1 */

/* ToyRing_SpawnSixC - spawner (5 blocks) of a six-figure toy ring of stiffness $80 (radius $32),
 * energy $C0, no drop action, its own movement coroutine ($03952C); it splits into single figures
 * later (coroutine 1 $039612)
 * In:  d4/d5 = position (wave spawn entry)
 * Out: jumps to Obj_SpawnEnemyN with a2 = Scr2_0392A0, d7 = 5
 * Evidence: the stub of ToyRing_SpawnSix / ToyRing_SpawnSixB; the wave spawn entries at $034CBC and
 *           $034CC4 name it (the wave of ToyRing_SpawnSixB's $034CCC), through VM5_SpawnEntry's
 *           jsr (a0) $007016; ToyRing_Init words 6 $32 $80 1 $9248. No corpus run calls it;
 *           039290.syn checks it.
 */
void ToyRing_SpawnSixC(uint16_t *d1, uint16_t x, uint16_t y, uint16_t *blocks, uint32_t *script, uint32_t *a3,
                       uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_0392A0;
    *blocks = TOYRING_BLOCKS;
    dk::jump(at_Obj_SpawnEnemyN);
}
