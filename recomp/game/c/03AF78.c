#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_03AF7C = 0x03AF7Cu;

/* ToyChain_MakeHanging8B - chain-child spawner of Enemy_PropToy_SpawnTwinChain8 (both chains):
 * ToyChain_SpawnInline with 8 links, table $03B190 and script $03AF86 (random speed $33-$E6, slow sway
 * oscillator $500 at $80)
 * In:  a6 = the holder (field $2A or $2C of the Op2_SpawnChild that names it); the inline arguments
 *      after the jsr
 * Out: d0 = 0 and the chain object, or nonzero (pool full)
 * Evidence: jsr ToyChain_SpawnInline + `0008 0003B190 0003AF86`; Op2_SpawnChild $2A and $2C dc.l at
 *           $03AB8C / $03AB98 in Enemy_PropToy_SpawnTwinChain8's script; no corpus run reaches it
 *           (raw-stubs 2026-10-30); ToyChain_SpawnInline pops the return address as its argument
 *           pointer and returns to this function's caller; the contract is ToyChain_MakeHanging6B's
 *           (the same call); 03AF78.syn checks it
 */
void ToyChain_MakeHanging8B(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4, uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_03AF7C);
}
