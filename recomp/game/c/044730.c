#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_04473C = 0x04473Cu;

/* Enemy_HoverPod_Spawn3C - spawns a hall hover pod on course 3 ($44892) with attack C ($44E82, eight spaced
 * shots), x speed $200
 * In:  x/y (d4/d5) = the spawn position (wave entry, screen x $A0)
 * Out: script (a2) = Scr2_04473C, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_04473C,a2 / jmp Obj_SpawnEnemy; Scr2_04473C: coroutine 0 $44892, coroutine 1 $44E82;
 *           wave entries Tbl_041138 +$FE, +$1A6, +$21C; TAS frame 117760
 */
void Enemy_HoverPod_Spawn3C(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                            uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_04473C;
    dk::jump(at_Obj_SpawnEnemy);
}
