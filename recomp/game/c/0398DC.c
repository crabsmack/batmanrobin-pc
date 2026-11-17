#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_0398E0 = 0x0398E0u;

/* ToyChain_MakeChild6 - chain-child spawner for Op2_SpawnChild: ToyChain_SpawnInline with 6 links,
 * table $039AC4, script $0398EA (sway speed $255, chain body $0379DA)
 * In:  d4/d5 = position
 * Out: d0 = 0 / a0 = chain block, or nonzero
 * Evidence: jsr ToyChain_SpawnInline(pc) / dc.w 6 / dc.l $039AC4 / dc.l $0398EA; dc.l $0398DC in
 *           Scr2_0397CA (ToyChain_Spawn6); TAS f84217-f85120 (4 frames); ToyChain_SpawnInline pops
 *           the return address as its argument pointer and returns to this function's caller
 */
void ToyChain_MakeChild6(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4,
                         uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_0398E0);
}
