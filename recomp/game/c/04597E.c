#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_04598A = 0x04598Au;

/* Enemy_VentTank_SpawnSlow - spawns the lab's vent tank (a tracked machine with five nozzles on top) at floor
 * height $9E, slow cruise $80: it drives at the targeted player, stops, then vents
 * gas puffs from its nozzles for $F00 ticks and rests $780, repeating
 * In:  x/y (d4/d5) = the spawn position (trigger entry)
 * Out: script (a2) = Scr2_04598A, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_04598A,a2 / jmp Obj_SpawnEnemy; Scr2_04598A: score $C8, energy $300, x accel/cruise $80,
 *           coroutine 0 $459DA (gosub $41D50 chase, wait $1E0, accel 0, vent gosub $45AF2, wait $F00, gosub
 *           $42E0C (anim $42E24, empty coroutine 1: venting off), wait $780), body $42D74 (population +3,
 *           attack box EnemyAtk_HitKnockSpeed $20/$200, treads by Stage4_TreadFrameByX); vents =
 *           Fx_SpawnAtOfs (0, -$18) with $45B8C, puffs hit through Enemy_VentTank_PuffHit; Tbl_041000 trigger
 *           entries x 3680/3936/4192 (spawn action $12); shot w29_c121230; TAS 121095-121263 (3 spawns)
 */
void Enemy_VentTank_SpawnSlow(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *script, uint32_t *a4,
                              uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_04598A;
    dk::jump(at_Obj_SpawnEnemy);
}
