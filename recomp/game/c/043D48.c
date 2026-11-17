#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_043D54 = 0x043D54u;

/* Enemy_GasOrb_SpawnBossTripleJetsFar - spawns a gas orb during the Mr. Freeze fight (jittered, 150 px
 * higher) with the jet-sequence / all-three-jets attacks of the
 * faster-puff set ($43F7E / $43EEE)
 * In:  x/y (d4/d5) = the spawn position
 * Out: script (a2) = Scr2_043D54, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_043D54,a2 / jmp Obj_SpawnEnemy; Scr2_043D54: as Enemy_GasOrb_SpawnBossPairJets with
 *           coroutine 1 $43DEC; spawner long at $46F68, $46F8A, $47084 in the Freeze fight scripts; TAS frame
 *           129202, orb with all three jets beside Mr. Freeze in shot w29_s129360
 */
void Enemy_GasOrb_SpawnBossTripleJetsFar(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7,
                                         uint32_t *script, uint32_t a4, uint32_t a6)
{
    *script = Scr2_043D54;
    dk::jump(at_Obj_SpawnEnemy);
}
