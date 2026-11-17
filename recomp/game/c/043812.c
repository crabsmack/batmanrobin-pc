#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_04381E = 0x04381Eu;

/* Enemy_GasOrb_SpawnTripleJetsFar - spawns a tube-section gas orb with the jet-sequence / all-three-jets
 * attacks of the faster-puff set ($43F7E / $43EEE, cruise y $1400)
 * In:  x/y (d4/d5) = the spawn position (wave entry)
 * Out: script (a2) = Scr2_04381E, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_04381E,a2 / jmp Obj_SpawnEnemy; Scr2_04381E: coroutine 1 $438AC picks $43F7E or $43EEE;
 *           wave entries at $41D34/$41D3C; TAS frame 127254
 */
void Enemy_GasOrb_SpawnTripleJetsFar(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                                     uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_04381E;
    dk::jump(at_Obj_SpawnEnemy);
}
