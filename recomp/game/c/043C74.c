#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_043C80 = 0x043C80u;

/* Enemy_GasOrb_SpawnBossPairJetsFar - spawns a gas orb during the Mr. Freeze fight (jittered, 150 px
 * higher) with the two-jet / single-jet sweeps of the faster-puff set
 * ($43E1C / $43E66, cruise y $1400)
 * In:  x/y (d4/d5) = the spawn position
 * Out: script (a2) = Scr2_043C80, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_043C80,a2 / jmp Obj_SpawnEnemy; Scr2_043C80: as Enemy_GasOrb_SpawnBossPairJets's
 *           Scr2_043AD8 with coroutine 1 $43D18 (vector 1 = $43E1C or $43E66); spawner long at $046F46,
 *           $047098 in the Freeze fight scripts; reached by scenario-sec-4401-freeze (reference corpus,
 *           2026-10-30); the contract is its siblings' (the same Freeze script calls); 043C74.syn checks it
 */
void Enemy_GasOrb_SpawnBossPairJetsFar(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7,
                                       uint32_t *script, uint32_t a4, uint32_t a6)
{
    *script = Scr2_043C80;
    dk::jump(at_Obj_SpawnEnemy);
}
