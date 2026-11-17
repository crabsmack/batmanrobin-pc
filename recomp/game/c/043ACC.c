#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_043AD8 = 0x043AD8u;

/* Enemy_GasOrb_SpawnBossPairJets - spawns a gas orb during the Mr. Freeze fight, placed with a jitter and 150
 * px higher, with the two-jet / single-jet sweeps ($440FC / $44146, slower
 * puffs)
 * In:  x/y (d4/d5) = the spawn position
 * Out: script (a2) = Scr2_043AD8, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_043AD8,a2 / jmp Obj_SpawnEnemy; Scr2_043AD8: Op2_JitterPos $12C/$28, op $1640 (y move)
 *           $96, coroutine 1 $43B70, drop init $438DC, body $41E4A; spawner long in the Freeze fight scripts
 *           at $46F1E, $47004, $470A8, $470BC (Scr2_046D32 / Scr2_046F7C); scenario-level-skip frame 14527
 *           only
 */
void Enemy_GasOrb_SpawnBossPairJets(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7,
                                    uint32_t *script, uint32_t a4, uint32_t a6)
{
    *script = Scr2_043AD8;
    dk::jump(at_Obj_SpawnEnemy);
}
