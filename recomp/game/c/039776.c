#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_039782 = 0x039782u;

/* ToyChain_Spawn7C - spawner of a toy-chain base carrying a 7-link chain (child $039A66,
 * ToyChain_MakeChild7C) with charge cycle $0385B4 and shedding schedule $0385F0 (a slow-swaying chain
 * of its own)
 * In:  d4/d5 = position (wave entry)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_039782
 * Evidence: Scr2_039782: Op2_SpawnChild $2A, Op2_StartChildCoroutine3Rel $2A $0385B4,
 *           Op2_StartChildCoroutine0Rel $2A $0385F0, Goto the base loop Scr2_0379BC; named by the wave
 *           spawn entries at $035014 (x -$28, y $E0C2) and $03501C (x $168, y $E0C2) (VM5_SpawnEntry's
 *           jsr (a0) $007016). No corpus run calls it at 4:3; widescreen_gameplay=1 may reach its wave
 *           step (ws-tail 2026-10-30); 039776.syn checks it
 */
void ToyChain_Spawn7C(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                      uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_039782;
    dk::jump(at_Obj_SpawnEnemy);
}
