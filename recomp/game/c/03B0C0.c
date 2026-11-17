#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_03B0C4 = 0x03B0C4u;

/* ToyChain_MakeHanging10C - first chain of the twin-chain propeller toy: ToyChain_SpawnInline with
 * 10 links, table $03B1E4 (link energy $20) and script $03B0CE (sway oscillator $AA, charge cycle
 * $038828, coroutine 0 $03823A, chain body $03802E)
 * In:  d4/d5 = position (from Op2_SpawnChild)
 * Out: d0 = 0 / a0 = chain block, or nonzero when the pool is full
 * Evidence: jsr ToyChain_SpawnInline(pc) / dc.w $A / dc.l $03B1E4 / dc.l $03B0CE; dc.l $03B0C0 in
 *           Scr2_03AE52+8 (child field $2A); TAS f97692; ToyChain_SpawnInline pops the return
 *           address as its argument pointer and returns to this function's caller
 */
void ToyChain_MakeHanging10C(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4, uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_03B0C4);
}
