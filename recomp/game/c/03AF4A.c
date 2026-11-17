#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_03AF4E = 0x03AF4Eu;

/* ToyChain_MakeHanging4 - chain-child spawner of Enemy_PropToy_SpawnChain4Right: ToyChain_SpawnInline
 * with 4 links, table $03B190 and script $03AF58 (speed $100, sway oscillator $800 at $12A)
 * In:  a6 = the holder (field $2A or $2C of the Op2_SpawnChild that names it); the inline arguments
 *      after the jsr
 * Out: d0 = 0 and the chain object, or nonzero (pool full)
 * Evidence: jsr ToyChain_SpawnInline + `0004 0003B190 0003AF58`; Op2_SpawnChild $2A dc.l at $03AB2E in
 *           Enemy_PropToy_SpawnChain4Right's script; no corpus run reaches it (raw-stubs 2026-10-30);
 *           ToyChain_SpawnInline pops the return address as its argument pointer and returns to this
 *           function's caller; the contract is ToyChain_MakeHanging6B's (the same call); 03AF4A.syn
 *           checks it
 */
void ToyChain_MakeHanging4(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4, uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_03AF4E);
}
