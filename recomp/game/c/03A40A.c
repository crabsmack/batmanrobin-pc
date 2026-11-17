#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_03A40E = 0x03A40Eu;

/* ToyChain_MakeBox5 - inline chain maker of a 5-figure chain (table $03A566: growth $200, energy
 * $10), sway oscillator $C00 at speed $180: one of the four random picks of
 * Enemy_GiftBox_SpawnRoamRandom
 * In:  a6 = the gift box (field $2A); inline dc.w 5, dc.l $03A566, dc.l $03A418
 * Out: d0 = 0 and the chain object, or nonzero (pool full)
 * Evidence: jsr ToyChain_SpawnInline + `0005 0003A566 0003A418`; Op2_SpawnChild 002A at $03A142 in
 *           the random subroutine $03A11C (Op2_SwitchRandom); TAS f83581, the pick the TAS drew.
 *           The 6-figure pick after it ($03A430, speed $1D5) is never run, still raw;
 *           ToyChain_SpawnInline pops the return address as its argument pointer and returns to
 *           this function's caller
 */
void ToyChain_MakeBox5(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4,
                       uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_03A40E);
}
