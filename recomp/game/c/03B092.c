#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_03B096 = 0x03B096u;

/* ToyChain_MakeHanging6D - chain-child spawner of Enemy_PropToy_SpawnTwinChain6's second chain:
 * ToyChain_SpawnInline with 6 links, table $03B1C8 and script $03B0A0 (speed $E6, sway oscillator $800
 * at $12A)
 * In:  a6 = the holder (field $2A or $2C of the Op2_SpawnChild that names it); the inline arguments
 *      after the jsr
 * Out: d0 = 0 and the chain object, or nonzero (pool full)
 * Evidence: jsr ToyChain_SpawnInline + `0006 0003B1C8 0003B0A0`; Op2_SpawnChild $2C dc.l at $03ADD6 in
 *           Enemy_PropToy_SpawnTwinChain6's script; no corpus run reaches it (raw-stubs 2026-10-30);
 *           ToyChain_SpawnInline pops the return address as its argument pointer and returns to this
 *           function's caller; the contract is ToyChain_MakeHanging6B's (the same call); 03B092.syn
 *           checks it
 */
void ToyChain_MakeHanging6D(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4, uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_03B096);
}
