#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_0391BA = 0x0391BAu;

enum { TOYRING_BLOCKS = 5 };   /* pool blocks for the ring: the usual 4 (Obj_SpawnEnemy) + 1 */

/* ToyRing_SpawnSixB - spawner (5 blocks) of a six-figure toy ring (radius $32, stiffness $60),
 * energy $C0, no drop action; it stops, hops in place, then rolls in bouts of 3 hops at cruise x
 * $2AA (coroutine 2 $0393CA) and splits into single figures later (coroutine 1 $039612)
 * In:  d4/d5 = position (wave spawn entry)
 * Out: jumps to Obj_SpawnEnemyN with a2 = Scr2_0391BA, d7 = 5
 * Evidence: the spawner stub of its siblings ToyRing_SpawnRandom / ToyRing_SpawnSix; named by the
 *           wave spawn entries at $0344B4, $0344BC and $034CCC (VM5_SpawnEntry's jsr (a0) $007016);
 *           its script = Scr2_0391F4 less Op2_SetSpawnAction $25, coroutine 2 $0393CA for $03941E.
 *           No corpus run reaches it at 4:3; widescreen=16:9-gameplay does (scenario-level-skip,
 *           sync 8391). Checked by 0391AA.syn.
 */
void ToyRing_SpawnSixB(uint16_t *d1, uint16_t x, uint16_t y, uint16_t *blocks, uint32_t *script, uint32_t *a3,
                       uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_0391BA;
    *blocks = TOYRING_BLOCKS;
    dk::jump(at_Obj_SpawnEnemyN);
}
