#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_04458C = 0x04458Cu;

/* Enemy_HoverPod_Spawn3A - spawns a hall hover pod on course 3 ($44892: a second pair of waypoint
 * subroutines) with attack A ($44D68, rounds of six aimed shots), x speed $200
 * In:  x/y (d4/d5) = the spawn position (wave entry, screen x $A0)
 * Out: script (a2) = Scr2_04458C, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_04458C,a2 / jmp Obj_SpawnEnemy; Scr2_04458C: coroutine 0 $44892, coroutine 1 $44D68, x
 *           $200, y $155; wave entries Tbl_041138 +$EE, +$196, +$20C; TAS 118543-118791 (2 spawns)
 */
void Enemy_HoverPod_Spawn3A(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                            uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_04458C;
    dk::jump(at_Obj_SpawnEnemy);
}
