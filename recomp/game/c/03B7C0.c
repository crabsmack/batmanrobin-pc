#include "game.h"

/* the a4 effect script (ROM data after the natives; batmanrobin.asm) */
static constexpr uint32_t Scr4_03B812 = 0x03B812u;

/* Boss_CatHead_EyeShotAt - the eye-shot natives' middle: the shot script Scr4_03B812 (sound $58, damage $10, radius velocity, terrain move), then the shared tail
 * In:  a6 = the head, a3 = its motion block, a2 = the Op2_CallNativeRel operand (word: the heading offset); d0's low word = the eye's x offset
 * Out: a0 = Scr4_03B812; then Boss_CatHead_EyeFxAt's
 * Evidence: lea Scr4_03B812,a0, then the next file Boss_CatHead_EyeFxAt; the bra.s of Boss_CatHead_EyeShotLeft and the fall-through of Boss_CatHead_EyeShotRight; no corpus run reaches it (raw-stubs 2026-10-30); 03B7C0.syn checks it
 */
void Boss_CatHead_EyeShotAt(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                            uint32_t d6, uint32_t d7, uint32_t *a0, uint32_t a1, uint32_t *a2, uint32_t a3,
                            uint32_t a4, uint32_t a5, uint32_t a6)
{
    *a0 = Scr4_03B812;
    dk::jump(at_Boss_CatHead_EyeFxAt);
}
