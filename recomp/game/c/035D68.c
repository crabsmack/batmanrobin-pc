#include "game.h"

/* the rabbit's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_035D74 = 0x035D74u;

/* Enemy_BombRabbit_SpawnHopFar - spawner of a faster bomb rabbit (Scr2_035D74: cruise $280, a
 * higher jump $78/$1E0, and a wide touch box that lights its fuse when a player comes near)
 * In:  x/y = spawn position
 * Out: script = Scr2_035D74, then Obj_SpawnEnemy's (d0 = 0 and a3/a4/a6 = the new rabbit's blocks;
 *      d0 nonzero when the pool is short)
 * Evidence: lea Scr2_035D74,a2 / jmp Obj_SpawnEnemy; the rabbit pot's Spawn_AtOwnOfs entries
 *           ($0367FA, $03680E) and $038938/$038948; TAS f111825. The prototype hands on a2 (and
 *           a3/a4/a6, as SpawnHop's): dk::jump passes only the prototype's registers, and
 *           Obj_SpawnEnemy reads a2
 */
void Enemy_BombRabbit_SpawnHopFar(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7,
                                  uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_035D74;
    dk::jump(at_Obj_SpawnEnemy);
}
