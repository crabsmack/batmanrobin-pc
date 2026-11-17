#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_038BE6 = 0x038BE6u;

/* ToyFigure_SpawnE - spawner of a single toy figure with coroutine 3 $03811E (as ToyFigure_SpawnD) and hop
 * coroutine $038DE2; body $036BCE
 * In:  d4/d5 = position (from the caller: Spawn_AtAnchor / Spawn_AtOwnOfs)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_038BE6
 * Evidence: ToyFigure_Spawn's stub; the dc.l operand of Op2_CallNative $008D52 at $038A04. No 4:3 corpus run calls it
 *           (widescreen=16:9-gameplay reaches ToyFigure_SpawnG); 038BDA.syn checks it.
 */
void ToyFigure_SpawnE(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6,
                      uint32_t *script)
{
    *script = Scr2_038BE6;
    dk::jump(at_Obj_SpawnEnemy);
}
