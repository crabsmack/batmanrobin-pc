#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_036422 = 0x036422u;

/* Enemy_PuffPlant_SpawnH - spawner of a puff plant (Scr2_036422, body $0355D4) that every $1E0 ticks
 * opens and puffs 20 puffs (the puff script its vector 0 names), as Enemy_PuffPlant_SpawnA's $168-tick
 * cycle of 10
 * In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
 * Out: script (a2) = Scr2_036422, then Obj_SpawnEnemy's results
 * Evidence: lea Scr2_036422,a2 / jmp Obj_SpawnEnemy; Scr2_036422: StartCoroutine0Rel $03642A (wait
 *           $1E0, counter $14, SetVector0HereGoto), Goto the puff plant body $0355D4; the world-placed
 *           wave entries at $0354DC and $0354E4 (x $B6E / $BAC, y $20B2) name it; no corpus run reaches
 *           it (raw-stubs 2026-10-30); the contract is Enemy_RabbitPot_SpawnA's (the same kind of
 *           call); 036416.syn checks it
 */
void Enemy_PuffPlant_SpawnH(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_036422;
    dk::jump(at_Obj_SpawnEnemy);
}
