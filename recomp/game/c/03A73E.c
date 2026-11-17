#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_03A742 = 0x03A742u;

/* ToyChain_MakeHanging6 - inline chain maker of a 6-figure chain that hangs from a propeller toy
 * (table $03A7F6, energy 1; body $03802E sways on oscillator $700/$12A/$89): coroutine 3 = charge
 * cycle $0380A8 (warning blink, then charged), coroutine 0 = $038286: after $5A0 ticks it sheds its
 * last figure as a falling toy figure ($03A91C via Spawn_AtAnchor) every $780-$F00 ticks
 * In:  a6 = the propeller toy (field $2A); inline dc.w 6, dc.l $03A7F6, dc.l $03A74C
 * Out: d0 = 0 and the chain object, or nonzero (pool full)
 * Evidence: jsr ToyChain_SpawnInline + `0006 0003A7F6 0003A74C`; script Op2_StartCoroutine3Rel ->
 *           $0380A8 (vector 1 -> $038A90 Op2_OrVar0 8, wait $2D0, Op2_OrVar0 4),
 *           Op2_StartCoroutine0Rel -> $038286 (vector 2 -> $038A64: ToyChain_RemoveLastLink +
 *           Spawn_AtAnchor $03A91C); Op2_SpawnChild 002A in
 *           Enemy_PropToy_SpawnChainRandom/ChainHover; TAS f101686-f107918 (40 runs);
 *           ToyChain_SpawnInline pops the return address as its argument pointer and returns to
 *           this function's caller
 */
void ToyChain_MakeHanging6(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4,
                           uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_03A742);
}
