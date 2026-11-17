#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_03A928 = 0x03A928u;

/* ToyFigure_SpawnWithDrop - spawner of a single toy figure shed from a chain, carrying a random
 * health drop: charge cycle $0380A8 (coroutine 3), big-hop coroutine $03A96E (coroutine 2), gosub
 * $03B11C picks spawn action $29/$2B/$2D/$2E or none, then the figure body $036BCE
 * In:  d4/d5 = position (the chain anchor, from Spawn_AtAnchor)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_03A928
 * Evidence: dc.l $03A91C after Spawn_AtAnchor in the shedding schedules Scr2_0386CC+$3A6 and
 *           Scr2_03A898+$7C; $03B11C = Op2_SwitchRandom into Op2_SetSpawnAction $29/$2B/$2D/$2E
 *           (Drop_* actions); ctarget in 7 corpus runs, no function-start frames in the TAS
 *           a2 an output (measured as a clobber): Obj_SpawnEnemy, jumped to, takes the script in it
 */
void ToyFigure_SpawnWithDrop(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script)
{
    *script = Scr2_03A928;
    dk::jump(at_Obj_SpawnEnemy);
}
