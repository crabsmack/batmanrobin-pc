#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_039716 = 0x039716u;

/* ToyChain_Spawn10D - spawner of a toy-chain base carrying a 10-link chain (child $039A06,
 * ToyChain_MakeChild10) with charge cycle $038578 and shedding schedule $038788 (the script of
 * ToyChain_Spawn10C, its own copy)
 * In:  d4/d5 = position (wave entry)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_039716
 * Evidence: Scr2_039716: Op2_SpawnChild $2A, Op2_StartChildCoroutine3Rel $2A $038578,
 *           Op2_StartChildCoroutine0Rel $2A $038788, Goto the base loop Scr2_0379BC; named by the wave
 *           spawn entry at $034686 (x $154, y $E09C) (VM5_SpawnEntry's jsr (a0) $007016). No corpus run
 *           calls it at 4:3; widescreen_gameplay=1 may reach its wave step (ws-tail 2026-10-30);
 *           03970A.syn checks it
 */
void ToyChain_Spawn10D(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                       uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_039716;
    dk::jump(at_Obj_SpawnEnemy);
}
