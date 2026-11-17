#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_03B0F2 = 0x03B0F2u;

/* ToyChain_MakeHanging10D - second chain of the twin-chain propeller toy: ToyChain_SpawnInline with
 * 10 links, table $03B200 (link energy $20) and script $03B0FC (sway oscillator $D5, charge cycle
 * $038840, coroutine 0 $03824E, chain body $03802E)
 * In:  d4/d5 = position (from Op2_SpawnChild)
 * Out: d0 = 0 / a0 = chain block, or nonzero when the pool is full
 * Evidence: jsr ToyChain_SpawnInline(pc) / dc.w $A / dc.l $03B200 / dc.l $03B0FC; dc.l $03B0EE in
 *           Scr2_03AE52+$14 (child field $2C); TAS f97692; ToyChain_SpawnInline pops the return
 *           address as its argument pointer and returns to this function's caller
 */
void ToyChain_MakeHanging10D(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4, uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_03B0F2);
}
