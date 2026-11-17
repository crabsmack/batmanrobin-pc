#include "game.h"

/* the boss coroutine after the native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_025D94 = 0x025D94u;

/* Boss_Stage2_ExplosionDue - the carry exit of Boss_Stage2_TickExplosions: back into the
 * coroutine at the explosion step (Op2_SwitchRandom picks one of three hull spots, an effect
 * spawns there)
 * In:  the running object's registers (for the script)
 * Out: the script continues at Scr2_025D94 (jsr $7FE.w)
 * Evidence: jsr $7FE.w alone; bcs target; Scr2_025D94: Op2_SwitchRandom, Op2_FxPosHere +
 *           Op2_FxPosAddW + Op2_FxSpawnRegs; TAS f33636-f34682 (490 frames)
 */
void Boss_Stage2_ExplosionDue(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                              uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    dk::jump_script(at_VM2_Enter, Scr2_025D94);
}
