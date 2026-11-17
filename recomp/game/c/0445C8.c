#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_0445D4 = 0x0445D4u;

/* Enemy_HoverPod_Spawn1B - spawns a hall hover pod on course 1 ($44836) with attack B ($44E14: three bursts
 * of seven shots $30 ticks apart), slower y speed $100
 * In:  x/y (d4/d5) = the spawn position (wave entry, screen x $A0)
 * Out: script (a2) = Scr2_0445D4, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_0445D4,a2 / jmp Obj_SpawnEnemy; Scr2_0445D4: x $2AA, y accel/cruise $100, coroutine 0
 *           $44836, coroutine 1 $44E14; wave entries Tbl_041138 +$76, +$AA, +$16E, +$1E4; 2 fuzz-117000 runs
 *           only (f118993, f119175)
 */
void Enemy_HoverPod_Spawn1B(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                            uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_0445D4;
    dk::jump(at_Obj_SpawnEnemy);
}
