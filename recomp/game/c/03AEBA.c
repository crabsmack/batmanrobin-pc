#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_03AEBE = 0x03AEBEu;

/* ToyChain_MakeHanging5E - chain-child spawner of Enemy_PropToy_SpawnChain5RightC: ToyChain_SpawnInline
 * with 5 links, table $03B158 and script $03AEC8 (random speed $33-$E6, sway oscillator $800 at $12A)
 * In:  a6 = the holder (field $2A or $2C of the Op2_SpawnChild that names it); the inline arguments
 *      after the jsr
 * Out: d0 = 0 and the chain object, or nonzero (pool full)
 * Evidence: jsr ToyChain_SpawnInline + `0005 0003B158 0003AEC8`; Op2_SpawnChild $2A dc.l at $03ABF2 in
 *           Enemy_PropToy_SpawnChain5RightC's script; no corpus run reaches it (raw-stubs 2026-10-30);
 *           ToyChain_SpawnInline pops the return address as its argument pointer and returns to this
 *           function's caller; the contract is ToyChain_MakeHanging6B's (the same call); 03AEBA.syn
 *           checks it
 */
void ToyChain_MakeHanging5E(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4, uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_03AEBE);
}
