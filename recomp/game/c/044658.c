#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_044664 = 0x044664u;

/* Enemy_HoverPod_Spawn3B - spawns a hall hover pod on course 3 ($44892) with attack B ($44E14, bursts of
 * seven shots), the fastest x speed $300
 * In:  x/y (d4/d5) = the spawn position (wave entry, screen x $A0)
 * Out: script (a2) = Scr2_044664, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_044664,a2 / jmp Obj_SpawnEnemy; Scr2_044664: x accel/cruise $300, y $100, coroutine 0
 *           $44892, coroutine 1 $44E14; wave entries Tbl_041138 +$F6, +$19E, +$214; TAS 117579-118668 (2
 *           spawns)
 */
void Enemy_HoverPod_Spawn3B(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                            uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_044664;
    dk::jump(at_Obj_SpawnEnemy);
}
