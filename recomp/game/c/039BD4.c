#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_039BD8 = 0x039BD8u;

/* ToyChain_MakeDropIn12 - inline chain maker of ToyChain_SpawnDropIn12: 12 figures (table $039C0C,
 * energy $B0) on the chain body $037B8A (sway cruise $155)
 * In:  a6 = the drop-in base (field $2A); inline dc.w $C, dc.l $039C0C, dc.l $037B8A
 * Out: d0 = 0 and the chain object, or nonzero (pool full)
 * Evidence: jsr ToyChain_SpawnInline + `000C 00039C0C 00037B8A`; Op2_SpawnChild 002A in
 *           Scr2_039B84; TAS f92263-f92886 (6 runs); ToyChain_SpawnInline pops the return address
 *           as its argument pointer and returns to this function's caller
 */
void ToyChain_MakeDropIn12(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4,
                           uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_039BD8);
}
