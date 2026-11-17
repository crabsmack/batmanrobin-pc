#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_038C92 = 0x038C92u;

/* ToyFigure_SpawnCharging - spawner of a single toy figure that starts its charge cycle at once
 * (coroutine 3 $0380F8: blink, charged about 1 s, calm 2 s, repeat) with the standard hops
 * ($038CC2); body $036BCE
 * In:  d4/d5 = position
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_038C92
 * Evidence: dc.l $038C86 in a chain shedding routine (Scr2_0386CC+$294, Spawn_AtAnchor); TAS
 *           f93392-f93449 (2 frames); a2 an output (measured as a clobber): Obj_SpawnEnemy, jumped
 *           to, takes the script in it
 */
void ToyFigure_SpawnCharging(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4,
                             uint32_t a6, uint32_t *script)
{
    *script = Scr2_038C92;
    dk::jump(at_Obj_SpawnEnemy);
}
