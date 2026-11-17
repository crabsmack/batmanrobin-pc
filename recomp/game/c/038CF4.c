#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_038D00 = 0x038D00u;

/* ToyFigure_SpawnF - spawner of a single toy figure at speed $100 with the standard charge cycle $0380B8 and
 * hop coroutine $038D10, spawned by a toy ring's break-up; body $036BCE
 * In:  d4/d5 = position (from the caller: Spawn_AtAnchor / Spawn_AtOwnOfs)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_038D00
 * Evidence: ToyFigure_Spawn's stub; the dc.l operand of Op2_CallNative $008D52 at $03960A (a toy ring's break-up script). No 4:3 corpus run calls it
 *           (widescreen_gameplay=1 reaches ToyFigure_SpawnG); 038CF4.syn checks it.
 */
void ToyFigure_SpawnF(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6,
                      uint32_t *script)
{
    *script = Scr2_038D00;
    dk::jump(at_Obj_SpawnEnemy);
}
