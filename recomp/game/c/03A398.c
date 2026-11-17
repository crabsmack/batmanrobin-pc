#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_03A39C = 0x03A39Cu;

/* ToyChain_MakeBox4 - inline chain maker of a 4-figure chain for the roaming gift boxes (table
 * $03A54A: growth $266, energy $20), sway oscillator limit $C00 at speed $255
 * In:  a6 = the gift box (field $2A); inline dc.w 4, dc.l $03A54A, dc.l $03A3A6
 * Out: d0 = 0 and the chain object, or nonzero (pool full)
 * Evidence: jsr ToyChain_SpawnInline + `0004 0003A54A 0003A3A6`; Op2_SpawnChild 002A in
 *           Enemy_GiftBox_SpawnRoam4A/4B ($03A048, $03A23C); TAS f83007 (section $3100). The
 *           random-pick makers after it ($03A3BE / $03A3E4: 4 figures, table $03A566, sway speed
 *           $D5 / $12A) are never run, still raw; ToyChain_SpawnInline pops the return address as
 *           its argument pointer and returns to this function's caller
 */
void ToyChain_MakeBox4(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4,
                       uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_03A39C);
}
