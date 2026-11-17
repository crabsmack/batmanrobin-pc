#include "game.h"

/* jetpack Harley's script in the warehouse (ROM, right after this file's code) */
static constexpr uint32_t Scr2_01EE64 = 0x01EE64u;

/* Boss_HarleyJet_Spawn - warehouse trigger callback: spawns Harley on her jetpack (boss class,
 * Scr2_01EE64: energy $250 / $450 with 2 players, waits until fewer than 2 other enemies, flies in)
 * In:  d4/d5 = x/y (the trigger's entry, Tbl_016632 +$50)
 * Out: script (a2) = Scr2_01EE64, then as Obj_SpawnBoss
 * Evidence: lea Scr2_01EE64,a2 / jmp Obj_SpawnBoss; Scr2_01EE64: SetEnergy1P2P $250/$450, score
 *           $7D0, IncPopulation, WaitPopBelow 2, a Camera_IsLocked switch; TAS f9904; shot f10860
 *           (Harley with a jetpack, tagged Scr2_01EE64)
 */
void Boss_HarleyJet_Spawn(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *a4, uint32_t *a5,
                          uint32_t *a6, uint32_t *script)
{
    *script = Scr2_01EE64;
    dk::jump(at_Obj_SpawnBoss);
}
