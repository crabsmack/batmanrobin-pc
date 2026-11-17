#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_03A7CC = 0x03A7CCu;

/* ToyChain_MakeHanging10 - inline chain maker of a 10-figure hanging chain (table $03A7F6, energy
 * 1; sway $700/$12A/$89; charge cycle $0380A8): coroutine 0 = $038262: after $9600 ticks it sheds
 * five figures at once (ToyFigure_SpawnStopHop via Spawn_AtAnchor), every $5A0 ticks
 * In:  a6 = the propeller toy (field $2A); inline dc.w $A, dc.l $03A7F6, dc.l $03A7D6
 * Out: d0 = 0 and the chain object, or nonzero (pool full)
 * Evidence: jsr ToyChain_SpawnInline + `000A 0003A7F6 0003A7D6`; $038262: timer $9600, five
 *           Op2_SetVector2HereGoto -> $038A4E (ToyChain_RemoveLastLink + Spawn_AtAnchor
 *           ToyFigure_SpawnStopHop), timer $5A0, loop; Op2_SpawnChild 002A in
 *           Enemy_PropToy_SpawnChain10; not in the TAS: fuzz-104400-s1/s2 f106141-f107945;
 *           ToyChain_SpawnInline pops the return address as its argument pointer and returns to
 *           this function's caller
 */
void ToyChain_MakeHanging10(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4, uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_03A7CC);
}
