#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_03AF1E = 0x03AF1Eu;

/* ToyChain_MakeHanging6B - chain-child spawner: ToyChain_SpawnInline with 6 links, table $03B174
 * (link energy $60) and script $03AF28 (charge cycle $038828, coroutine 0 $03822C, chain body
 * $03802E)
 * In:  d4/d5 = position (from Op2_SpawnChild)
 * Out: d0 = 0 / a0 = chain block, or nonzero when the pool is full
 * Evidence: jsr ToyChain_SpawnInline(pc) / dc.w 6 / dc.l $03B174 / dc.l $03AF28; dc.l $03AF1A in
 *           Scr2_03AA94+8 (Enemy_PropToy_SpawnChain6Right); ctarget in 1 corpus run;
 *           ToyChain_SpawnInline pops the return address as its argument pointer and returns to
 *           this function's caller
 */
void ToyChain_MakeHanging6B(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4, uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_03AF1E);
}
