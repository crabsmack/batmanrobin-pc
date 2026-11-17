#include "game.h"

/* the little hat object's a2 script (boss class, energy $120; ROM data after the spawner) */
static constexpr uint32_t Scr2_03D940 = 0x03D940u;

/* Boss_Hatter_SpawnLittleHat - spawns the little top hat that jumps out of the defeated hat,
 * bounces along the road, and drops an extra life when shot down
 * In:  d4/d5 = position (Spawn_AtOwnOfs: the boss + (0, -20))
 * Out: script (a2) = Scr2_03D940, then Obj_SpawnBoss's results
 * Evidence: lea Scr2_03D940,a2 / jmp Obj_SpawnBoss; Spawn_AtOwnOfs dc.l $0003D934 at $03D74A in
 *           Scr2_03D508; TAS f115594. a2 an output (measured as a clobber): Obj_SpawnBoss, jumped
 *           to, takes the script in it
 */
void Boss_Hatter_SpawnLittleHat(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7,
                                uint32_t *script, uint32_t a4, uint32_t a6)
{
    *script = Scr2_03D940;
    dk::jump(at_Obj_SpawnBoss);
}
