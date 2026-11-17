#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_0398BE = 0x0398BEu;

/* ToyChain_MakeChild5 - chain-child spawner for Op2_SpawnChild: ToyChain_SpawnInline with 5 links,
 * parameter table $039AC4 (shootable links, heading $300, $90 energy per link) and script $0398C8
 * (sway speed $280, then the chain body $0379DA)
 * In:  d4/d5 = position (from Op2_SpawnChild)
 * Out: d0 = 0 / a0 = chain block, or nonzero when the pool is full
 * Evidence: jsr ToyChain_SpawnInline(pc) / dc.w 5 / dc.l $039AC4 / dc.l $0398C8; dc.l $0398BA in
 *           Scr2_0397A6 (ToyChain_Spawn5); TAS f83916-f86444 (3 frames); ToyChain_SpawnInline pops
 *           the return address as its argument pointer and returns to this function's caller
 */
void ToyChain_MakeChild5(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4,
                         uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_0398BE);
}
