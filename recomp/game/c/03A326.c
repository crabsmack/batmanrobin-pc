#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_03A32A = 0x03A32Au;

/* ToyChain_MakeBox10C - inline chain maker for Enemy_GiftBox_SpawnStay10C: 10 figures (table $03A4F6)
 * that grow out of the opened box on the swaying body $037DD6 (oscillator X limit $1E00, speed $100)
 * In:  a6 = the holder (field $2A or $2C of the Op2_SpawnChild that names it); the inline arguments
 *      after the jsr
 * Out: d0 = 0 and the chain object, or nonzero (pool full)
 * Evidence: jsr ToyChain_SpawnInline + `000A 0003A4F6 0003A334`; Op2_SpawnChild $2A dc.l at $039C72 in
 *           Enemy_GiftBox_SpawnStay10C's coroutine 0; no corpus run reaches it (raw-stubs 2026-10-30);
 *           ToyChain_SpawnInline pops the return address as its argument pointer and returns to this
 *           function's caller; the contract is ToyChain_MakeBox10A's (the same call); 03A326.syn checks
 *           it
 */
void ToyChain_MakeBox10C(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4, uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_03A32A);
}
