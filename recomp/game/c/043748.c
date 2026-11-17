#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_043754 = 0x043754u;

/* Enemy_GasOrb_SpawnPairJetsFar - spawns a tube-section gas orb with the two-jet / single-jet sweeps of the
 * faster-puff set (cruise y $1400: $43E1C / $43E66), so its jets reach
 * farther
 * In:  x/y (d4/d5) = the spawn position (wave entry)
 * Out: script (a2) = Scr2_043754, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_043754,a2 / jmp Obj_SpawnEnemy; Scr2_043754: coroutine 1 $437E2 picks vector 1 = $43E1C
 *           or $43E66, which set Op2_SetCruiseY $1400 (the puff start speed read by Enemy_GasOrb_StepJet)
 *           where the $440FC set uses $A00; wave entries at $41D24/$41D2C; TAS frame 127243
 */
void Enemy_GasOrb_SpawnPairJetsFar(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                                   uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_043754;
    dk::jump(at_Obj_SpawnEnemy);
}
