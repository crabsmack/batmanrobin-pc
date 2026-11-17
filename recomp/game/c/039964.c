#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_039968 = 0x039968u;

/* ToyChain_MakeChild6B - inline chain maker of ToyChain_Spawn6B: a 6-figure fast-swaying chain
 * (energy $90), speed $33-$100, sway cruise $2AA, brake distance $50
 * In:  a6 = the base (ToyChain_Spawn6B, field $2A); inline dc.w 6, dc.l Tbl_039A0A+$BA, dc.l
 *      $039972
 * Out: d0 = 0 and the chain object, or nonzero (pool full)
 * Evidence: jsr ToyChain_SpawnInline + `0006 00039AC4 00039972`; Op2_SpawnChild 002A in Scr2_03985A
 *           (spawn entries Tbl_034142+$4D4); not in the TAS: fuzz-109800-s1/s2 f111750-f113080
 *           (section $3400); ToyChain_SpawnInline pops the return address as its argument pointer
 *           and returns to this function's caller
 */
void ToyChain_MakeChild6B(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4,
                          uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_039968);
}
