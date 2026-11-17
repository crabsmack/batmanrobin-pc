#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_0446F4 = 0x0446F4u;

/* Enemy_HoverPod_Spawn2C - spawns a hall hover pod on course 2 ($4487A) with attack C ($44E82, eight spaced
 * shots), x speed $200
 * In:  x/y (d4/d5) = the spawn position (wave entry, screen x $A0)
 * Out: script (a2) = Scr2_0446F4, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_0446F4,a2 / jmp Obj_SpawnEnemy; Scr2_0446F4: x $200/$60, y $155, coroutine 0 $4487A,
 *           coroutine 1 $44E82; wave entries Tbl_041138 +$E6, +$18E, +$204; TAS frame 118671
 */
void Enemy_HoverPod_Spawn2C(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                            uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_0446F4;
    dk::jump(at_Obj_SpawnEnemy);
}
