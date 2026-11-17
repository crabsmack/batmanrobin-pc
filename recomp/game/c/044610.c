#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_04461C = 0x04461Cu;

/* Enemy_HoverPod_Spawn2B - spawns a hall hover pod on course 2 ($4487A) with attack B ($44E14, bursts of
 * seven shots), x speed $2AA with the short accel distance $50
 * In:  x/y (d4/d5) = the spawn position (wave entry, screen x $A0)
 * Out: script (a2) = Scr2_04461C, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_04461C,a2 / jmp Obj_SpawnEnemy; Scr2_04461C: x accel $2AA/$50, y $100, coroutine 0
 *           $4487A, coroutine 1 $44E14; wave entries Tbl_041138 +$DE, +$186, +$1FC; TAS frame 118546
 */
void Enemy_HoverPod_Spawn2B(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                            uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_04461C;
    dk::jump(at_Obj_SpawnEnemy);
}
