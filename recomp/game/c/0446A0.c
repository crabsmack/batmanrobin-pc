#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_0446AC = 0x0446ACu;

/* Enemy_HoverPod_Spawn1C - spawns a hall hover pod on course 1 ($44836) with attack C ($44E82: eight shots
 * $3C ticks apart, then a long pause), x $2AA, y $155
 * In:  x/y (d4/d5) = the spawn position (wave entry, screen x $A0)
 * Out: script (a2) = Scr2_0446AC, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_0446AC,a2 / jmp Obj_SpawnEnemy; Scr2_0446AC: coroutine 0 $44836, coroutine 1 $44E82
 *           (turn rate $100, counter 8, Op2_FxSpawnRegs); wave entries Tbl_041138 +$7E, +$B2, +$176, +$1EC;
 *           TAS frame 117215
 */
void Enemy_HoverPod_Spawn1C(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                            uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_0446AC;
    dk::jump(at_Obj_SpawnEnemy);
}
