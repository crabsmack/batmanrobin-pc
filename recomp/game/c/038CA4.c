#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_038CB0 = 0x038CB0u;

/* ToyFigure_Spawn - spawner of the standard single toy figure (Scr2_038CB0): hops toward the
 * players (coroutine 2 $038CC2) and runs the charge cycle $0380B8 (2 s calm, warning blink, charged
 * about 1 s, calm, random pause); body $036BCE (energy $10, touch damage only while charged)
 * In:  d4/d5 = position (from the caller, e.g. Spawn_AtAnchor / Spawn_AtOwnOfs)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_038CB0
 * Evidence: dc.l $038CA4 in the drop carrier Scr2_0369DC (8 sites) and in the chain shedding
 *           routines of Scr2_0386CC ($03887C-$0388B8, 10 sites); scenario-level-skip f10561-f10621,
 *           TAS f92384-f110946 (11 frames); a2 an output (measured as a clobber): Obj_SpawnEnemy,
 *           jumped to, takes the script in it
 */
void ToyFigure_Spawn(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4,
                     uint32_t a6, uint32_t *script)
{
    *script = Scr2_038CB0;
    dk::jump(at_Obj_SpawnEnemy);
}
