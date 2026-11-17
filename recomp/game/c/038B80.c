#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_038B8C = 0x038B8Cu;

/* ToyFigure_SpawnC - spawner of a single toy figure with the charge schedule $0387DA and the pause-then-random-hop
 * coroutine $038D9A (ToyFigure_SpawnRandCharge's); body $036BCE
 * In:  d4/d5 = position (from the caller: Spawn_AtAnchor / Spawn_AtOwnOfs)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_038B8C
 * Evidence: ToyFigure_Spawn's stub; the dc.l operand of Op2_CallNative $008D52 at $0388F0, $038A1A, $03965C. No 4:3 corpus run calls it
 *           (widescreen_gameplay=1 reaches ToyFigure_SpawnG); 038B80.syn checks it.
 */
void ToyFigure_SpawnC(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6,
                      uint32_t *script)
{
    *script = Scr2_038B8C;
    dk::jump(at_Obj_SpawnEnemy);
}
