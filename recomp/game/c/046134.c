#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_046140 = 0x046140u;

/* Enemy_BombBot_SpawnHall - spawns a bomb bot in the stage 4 hall (floor height $B6, cruise $1AA, fuse timer
 * words $B40/$B40) with a drop picked at spawn
 * In:  x/y (d4/d5) = the spawn position (wave entry: x $154 or -$14)
 * Out: script (a2) = Scr2_046140, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_046140,a2 / jmp Obj_SpawnEnemy; Scr2_046140: population +1, drop init gosub $43A68
 *           (Op2_BranchWeaponLvlMaxGE + Op2_SwitchRandom -> Op2_SetSpawnAction $15/$09/$12 ...), score $C8,
 *           energy $100, body $432E2; sibling $04616A has no drop init; wave entries Tbl_041138 +$4C/+$54 and
 *           Scr5_0413AC +$C/+$14; RAM f117480 an object of this body at x 85 (camera 61): the antenna capsule
 *           robot of shot w29_c117480; TAS 116432-118602 (7 spawns)
 */
void Enemy_BombBot_SpawnHall(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                             uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_046140;
    dk::jump(at_Obj_SpawnEnemy);
}
