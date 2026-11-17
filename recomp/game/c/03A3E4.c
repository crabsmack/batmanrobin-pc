#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_03A3E8 = 0x03A3E8u;

/* ToyChain_MakeBox4C - inline chain maker of a 4-figure chain (table $03A566) for
 * Enemy_GiftBox_SpawnRoamRandom: sway oscillator limit $C00 at speed $12A
 * In:  a6 = the holder (field $2A or $2C of the Op2_SpawnChild that names it); the inline arguments
 *      after the jsr
 * Out: d0 = 0 and the chain object, or nonzero (pool full)
 * Evidence: jsr ToyChain_SpawnInline + `0004 0003A566 0003A3F2`; Op2_SpawnChild $2A dc.l at $03A138:
 *           the second random pick of Enemy_GiftBox_SpawnRoamRandom's subroutine $03A11C; no corpus run
 *           reaches it (raw-stubs 2026-10-30); ToyChain_SpawnInline pops the return address as its
 *           argument pointer and returns to this function's caller; the contract is ToyChain_MakeBox5's
 *           (the same call); 03A3E4.syn checks it
 */
void ToyChain_MakeBox4C(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4, uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_03A3E8);
}
