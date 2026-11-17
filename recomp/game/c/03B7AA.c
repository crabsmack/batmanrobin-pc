#include "game.h"

/* the a4 effect script (ROM data after the natives; batmanrobin.asm) */
static constexpr uint32_t Scr4_03B7F2 = 0x03B7F2u;

/* Boss_CatHead_EyeSparkRight - native of the cat-head boss's attack coroutines: a spark (Scr4_03B7F2) at the head's right eye (x +$1E)
 * In:  a6 = the head, a3 = its motion block, a2 = the Op2_CallNativeRel operand (word: the heading offset)
 * Out: d0's low word = +$1E, a0 = Scr4_03B7F2; then Boss_CatHead_EyeFxAt's
 * Evidence: move.w #$1E,d0 / lea Scr4_03B7F2,a0 / bra.s Boss_CatHead_EyeFxAt; Op2_CallNativeRel at $03C446 and $03C45E; no corpus run reaches it (raw-stubs 2026-10-30); 03B7AA.syn checks it
 */
void Boss_CatHead_EyeSparkRight(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                                uint32_t d6, uint32_t d7, uint32_t *a0, uint32_t a1, uint32_t *a2, uint32_t a3,
                                uint32_t a4, uint32_t a5, uint32_t a6)
{
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)(int16_t)0x1E;
    *a0 = Scr4_03B7F2;
    dk::jump(at_Boss_CatHead_EyeFxAt);
}
