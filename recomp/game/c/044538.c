#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_044544 = 0x044544u;

/* Enemy_HoverPod_Spawn2A - spawns a hall hover pod on course 2 ($4487A: alternating waypoint subroutines at a
 * fixed cruise) with attack A ($44D68, rounds of six aimed shots), x speed $200
 * In:  x/y (d4/d5) = the spawn position (wave entry, screen x $A0)
 * Out: script (a2) = Scr2_044544, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_044544,a2 / jmp Obj_SpawnEnemy; Scr2_044544: as Enemy_HoverPod_Spawn1A with x
 *           accel/cruise $200 (distance $60) and coroutine 0 $4487A; wave entries Tbl_041138 +$D6, +$17E,
 *           +$1F4; TAS 117398-118422 (3 spawns)
 */
void Enemy_HoverPod_Spawn2A(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                            uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_044544;
    dk::jump(at_Obj_SpawnEnemy);
}
