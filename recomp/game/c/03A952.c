#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_03A95E = 0x03A95Eu;

/* ToyFigure_SpawnShed - spawner of a single toy figure shed from a chain, as ToyFigure_SpawnWithDrop
 * without the drop gosub $03B11C: charge cycle $0380A8 (coroutine 3), big-hop coroutine $03A96E
 * (coroutine 2), body $036BCE
 * In:  d4/d5 = position (the chain anchor, from Spawn_AtAnchor)
 * Out: script (a2) = Scr2_03A95E, then Obj_SpawnEnemy's results
 * Evidence: lea Scr2_03A95E,a2 / jmp Obj_SpawnEnemy; Scr2_03A95E: SetSpeed $100, StartCoroutine3Rel
 *           $0380A8, StartCoroutine2Rel $03A96E, Goto $036BCE; dc.l of Op2_CallNative Spawn_AtAnchor
 *           ($008D52) at $03A946 in ToyFigure_SpawnWithDrop's script; no corpus run reaches it
 *           (raw-stubs 2026-10-30); the contract is ToyFigure_SpawnWithDrop's (the same kind of call);
 *           03A952.syn checks it
 */
void ToyFigure_SpawnShed(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script)
{
    *script = Scr2_03A95E;
    dk::jump(at_Obj_SpawnEnemy);
}
