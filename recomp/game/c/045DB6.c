#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_045DC2 = 0x045DC2u;

/* Enemy_GunTank_Spawn - spawns the lab's gun tank (a domed tracked tank with a twin-barrel turret) at floor
 * height $92: drives in at cruise $80, stops after $1E0 ticks and fires 5-shot bursts
 * ($50 ticks apart, every $5A0) from the turret aimed at the player
 * In:  x/y (d4/d5) = the spawn position (trigger entry)
 * Out: script (a2) = Scr2_045DC2, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_045DC2,a2 / jmp Obj_SpawnEnemy; Scr2_045DC2: score $C8, energy $100, x $80, gosub $43092
 *           (turret coroutine 3: Op2_AimAtTracked + Enemy_GunTank_ClampAimUp), coroutine 0 $45DEC, coroutine
 *           1 $45E10 (turn rate $AAA, counter 5, Op2_FxHeadingSnap $10 + Op2_FxSpawnRegs), body $42FEC
 *           (population +3, turret in the extra sprite block $10(a6)); Tbl_041000 trigger entries x 832-2624
 *           (7, spawn action $12); RAM f119700 two objects of this body, shot w29_c119760; TAS 119573-120149
 *           (7 spawns)
 */
void Enemy_GunTank_Spawn(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *script, uint32_t *a4,
                         uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_045DC2;
    dk::jump(at_Obj_SpawnEnemy);
}
