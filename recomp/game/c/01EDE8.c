#include "game.h"

/* jetpack Harley's script for the final fight (ROM, right after this file's code) */
static constexpr uint32_t Scr2_01EDF4 = 0x01EDF4u;

/* Boss_HarleyJet_SpawnFinal - claw-script native: spawns jetpack Harley for the final fight
 * (Scr2_01EDF4: energy $700 / $C00 with 2 players, BossMeter_Show, score 5000)
 * In:  d4/d5 = the position Spawn_AtCameraOfs gives (op $2CBC at $1E7F4, camera offset -$1E/$46)
 * Out: script (a2) = Scr2_01EDF4, then as Obj_SpawnBoss (d0 = 0 and a6 = Harley; Z clear when the
 *      pool is short)
 * Evidence: lea Scr2_01EDF4,a2 / jmp Obj_SpawnBoss; called from Scr2_01E778 +$82; TAS f11992, her
 *           object $BEE8 has energy $700 at f12060, the boss meter and BOSS TIME at f12420
 */
void Boss_HarleyJet_SpawnFinal(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a4,
                               uint32_t a6, uint32_t *script)
{
    *script = Scr2_01EDF4;
    dk::jump(at_Obj_SpawnBoss);
}
