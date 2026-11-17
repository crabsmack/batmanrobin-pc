#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_0454C2 = 0x0454C2u;

/* Enemy_JetTrooper_SpawnHoverLeft - spawns a tube-section jetpack trooper that holds a window left of the
 * targeted player (x -$50..-$B4), cruise $1AA, turn rate $3AA
 * In:  x/y (d4/d5) = the spawn position (wave entry, x $168)
 * Out: script (a2) = Scr2_0454C2, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_0454C2,a2 / jmp Obj_SpawnEnemy; Scr2_0454C2: coroutine 0 $454E8 with Op2_GosubArgs ($50,
 *           -$50, -$50, -$B4), waits $B40/$780; wave entry $41CB4; RAM f123660/f127740 slot 0 $45500; TAS
 *           123611-127731 (2 spawns)
 */
void Enemy_JetTrooper_SpawnHoverLeft(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                                     uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_0454C2;
    dk::jump(at_Obj_SpawnEnemy);
}
