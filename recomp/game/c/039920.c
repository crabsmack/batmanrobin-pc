#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_039924 = 0x039924u;

/* ToyChain_MakeChild8 - inline chain maker of ToyChain_Spawn8: an 8-figure chain (energy $90),
 * random speed $4C-$100, sway cruise $1AA (the slowest of the 5-8 link chains)
 * In:  a6 = the base (ToyChain_Spawn8, field $2A); inline dc.w 8, dc.l Tbl_039A0A+$BA, dc.l $03992E
 * Out: d0 = 0 and the chain object, or nonzero (pool full)
 * Evidence: jsr ToyChain_SpawnInline + `0008 00039AC4 0003992E`; Op2_SpawnChild 002A in Scr2_039812
 *           (spawn entry Scr5_034932+$262); TAS f86023, section $3100; ToyChain_SpawnInline pops
 *           the return address as its argument pointer and returns to this function's caller
 */
void ToyChain_MakeChild8(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4,
                         uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_039924);
}
