#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_045EDE = 0x045EDEu;

/* Enemy_GunTank_SpawnTough - spawns a tougher gun tank for the lab's last arena (energy $200, score $190,
 * random speed, cruise $1D5) that fires one 4-shot volley (shots $F0 ticks apart)
 * per round, then waits $3C0 + $B40 ticks
 * In:  x/y (d4/d5) = the spawn position (wave entry: x $17C or -$3C)
 * Out: script (a2) = Scr2_045EDE, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_045EDE,a2 / jmp Obj_SpawnEnemy; Scr2_045EDE: Op2_SetSpeedRand $33/$E6, x accel/cruise
 *           $1D5, coroutine 0 $45F06, coroutine 1 $45F1E (wait $B40, counter 1 = 1 so Op2_LoopCounter1 never
 *           loops, counter 0 = 4 shots, wait $3C0, goto the $B40 wait), Op2_PlaceYOnScreen $92, body $42FEC;
 *           wave entries $41A56/$41A5E (arena wave $41A62, after Op5_LockCameraHere); RAM f120720 an object
 *           of this body with slots $45F0E/$45F2A; TAS 120361-120707 (4 spawns)
 */
void Enemy_GunTank_SpawnTough(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                              uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_045EDE;
    dk::jump(at_Obj_SpawnEnemy);
}
