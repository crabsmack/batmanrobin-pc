#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_0398A2 = 0x0398A2u;

/* ToyChain_Spawn8B - spawner of a toy-chain base carrying an 8-link chain (child $0399A6,
 * ToyChain_MakeChild8B) with charge cycle $0380D8 and shedding schedule $03870C (the fixed-delay
 * schedules of ToyChain_Spawn5B-7B)
 * In:  d4/d5 = position (wave entry)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_0398A2
 * Evidence: Scr2_0398A2: Op2_SpawnChild $2A, Op2_StartChildCoroutine3Rel $2A $0380D8,
 *           Op2_StartChildCoroutine0Rel $2A $03870C, Goto the base loop Scr2_0379BC; named by the wave
 *           spawn entry at $034632 (x $154, y $E09C) (VM5_SpawnEntry's jsr (a0) $007016). No corpus run
 *           calls it at 4:3; widescreen=16:9-gameplay may reach its wave step (ws-tail 2026-10-30);
 *           039896.syn checks it
 */
void ToyChain_Spawn8B(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                      uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_0398A2;
    dk::jump(at_Obj_SpawnEnemy);
}
