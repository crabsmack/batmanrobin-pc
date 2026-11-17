#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_03A898 = 0x03A898u;

/* ToyChain_MakeHanging10B - inline chain maker of the 10-figure chain of
 * Enemy_PropToy_SpawnChainWaypoints (table $03A8C2, energy 1; sway $500/$AA/$89; charge cycle
 * $0380A8): its own coroutine 0 sheds the last figure ($03A91C via Spawn_AtAnchor) after $F00
 * ticks, then $1E0, $1E0 and $780 apart, repeating
 * In:  a6 = the propeller toy (field $2A); inline dc.w $A, dc.l $03A8C2, dc.l $03A8A2
 * Out: d0 = 0 and the chain object, or nonzero (pool full)
 * Evidence: jsr ToyChain_SpawnInline + `000A 0003A8C2 0003A8A2`; coroutine 0 $03A8DE:
 *           Op2_SetTimer2Yield0 $F00/$1E0/$1E0/$780 + Op2_SetVector2HereGoto -> $03A906
 *           (ToyChain_RemoveLastLink, Op2_CallNative Spawn_AtAnchor `0003A91C`); Op2_SpawnChild
 *           002A at $03A822; TAS f104480-f104884 (3 runs); ToyChain_SpawnInline pops the return
 *           address as its argument pointer and returns to this function's caller
 */
void ToyChain_MakeHanging10B(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4, uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_03A898);
}
