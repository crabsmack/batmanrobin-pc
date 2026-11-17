#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_03AFDC = 0x03AFDCu;

/* ToyChain_MakeHanging5B - chain-child spawner: ToyChain_SpawnInline with 5 links, table $03B1AC
 * (link energy 1) and script $03AFE6 (charge cycle $038810 only, no coroutine 0, chain body
 * $03802E)
 * In:  d4/d5 = position (from Op2_SpawnChild)
 * Out: d0 = 0 / a0 = chain block, or nonzero when the pool is full
 * Evidence: jsr ToyChain_SpawnInline(pc) / dc.w 5 / dc.l $03B1AC / dc.l $03AFE6; dc.l $03AFD8 in
 *           Scr2_03AC8E+8; TAS f94297-f95872; ToyChain_SpawnInline pops the return address as its
 *           argument pointer and returns to this function's caller
 */
void ToyChain_MakeHanging5B(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4, uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_03AFDC);
}
