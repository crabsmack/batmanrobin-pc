#include "game.h"

/* Boss_CatHead_EyeShotLeft - native of the cat-head boss's attack coroutines: a shot (Scr4_03B812: sound $58, damage $10) from the head's left eye (x -$1E)
 * In:  a6 = the head, a3 = its motion block, a2 = the Op2_CallNativeRel operand (word: the heading offset)
 * Out: d0's low word = -$1E; then Boss_CatHead_EyeShotAt's
 * Evidence: move.w #$FFE2,d0 / bra.s Boss_CatHead_EyeShotAt; Op2_CallNativeRel at $03C47C; no corpus run reaches it (raw-stubs 2026-10-30); 03B7B6.syn checks it
 */
void Boss_CatHead_EyeShotLeft(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                              uint32_t d6, uint32_t d7, uint32_t *a0, uint32_t a1, uint32_t *a2, uint32_t a3,
                              uint32_t a4, uint32_t a5, uint32_t a6)
{
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)(int16_t)-0x1E;
    dk::jump(at_Boss_CatHead_EyeShotAt);
}
