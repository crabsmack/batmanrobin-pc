#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_043BAC = 0x043BACu;

/* Enemy_GasOrb_SpawnBossTripleJets - spawns a gas orb during the Mr. Freeze fight (jittered, 150 px
 * higher) with the jet-sequence / all-three-jets attacks of the slower-puff set
 * ($4425E / $441CE, cruise y $A00)
 * In:  x/y (d4/d5) = the spawn position
 * Out: script (a2) = Scr2_043BAC, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_043BAC,a2 / jmp Obj_SpawnEnemy; Scr2_043BAC: as Enemy_GasOrb_SpawnBossPairJets's
 *           Scr2_043AD8 with coroutine 1 $43C44 (vector 1 = $4425E or $441CE); spawner long at $046FF4,
 *           $04701E in the Freeze fight scripts; no corpus run reaches it (converted with $043C74);
 *           the contract is its siblings'; 043BA0.syn checks it
 */
void Enemy_GasOrb_SpawnBossTripleJets(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7,
                                      uint32_t *script, uint32_t a4, uint32_t a6)
{
    *script = Scr2_043BAC;
    dk::jump(at_Obj_SpawnEnemy);
}
