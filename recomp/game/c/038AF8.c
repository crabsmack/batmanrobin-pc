#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_038B04 = 0x038B04u;

/* ToyFigure_SpawnB - spawner of a single toy figure with coroutine 3 $038074 and its own hop coroutine
 * $038B40 (random speed $33-$E6); body $036BCE
 * In:  d4/d5 = position (from the caller: Spawn_AtAnchor / Spawn_AtOwnOfs)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_038B04
 * Evidence: ToyFigure_Spawn's stub; the dc.l operand of Op2_CallNative $008D80 at $036B94, $036BA8, $036BBC. No 4:3 corpus run calls it
 *           (widescreen=16:9-gameplay reaches ToyFigure_SpawnG); 038AF8.syn checks it.
 */
void ToyFigure_SpawnB(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6,
                      uint32_t *script)
{
    *script = Scr2_038B04;
    dk::jump(at_Obj_SpawnEnemy);
}
