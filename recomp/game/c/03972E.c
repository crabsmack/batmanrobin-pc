#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_03973A = 0x03973Au;

/* ToyChain_Spawn10E - spawner of a toy-chain base carrying a 10-link chain (child $039A06,
 * ToyChain_MakeChild10) with charge cycle $038596 and shedding schedule $0387CC (ToyChain_Spawn10A's
 * charge cycle, its own shedding schedule)
 * In:  d4/d5 = position (wave entry)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_03973A
 * Evidence: Scr2_03973A: Op2_SpawnChild $2A, Op2_StartChildCoroutine3Rel $2A $038596,
 *           Op2_StartChildCoroutine0Rel $2A $0387CC, Goto the base loop Scr2_0379BC; named by the wave
 *           spawn entry at $03468E (x -$14, y $E09C) (VM5_SpawnEntry's jsr (a0) $007016). No corpus run
 *           calls it at 4:3; widescreen_gameplay=1 may reach its wave step (ws-tail 2026-10-30);
 *           03972E.syn checks it
 */
void ToyChain_Spawn10E(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                       uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_03973A;
    dk::jump(at_Obj_SpawnEnemy);
}
