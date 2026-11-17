#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_039BE6 = 0x039BE6u;

/* ToyChain_MakeDropIn9 - inline chain maker of ToyChain_SpawnDropIn9: 9 figures (table $039C28,
 * energy $90) on the chain body $037B8A
 * In:  a6 = the drop-in base (field $2A); inline dc.w 9, dc.l $039C28, dc.l $037B8A
 * Out: d0 = 0 and the chain object, or nonzero (pool full)
 * Evidence: jsr ToyChain_SpawnInline + `0009 00039C28 00037B8A`; Op2_SpawnChild 002A in
 *           Scr2_039BAC; TAS f93331-f93388; ToyChain_SpawnInline pops the return address as its
 *           argument pointer and returns to this function's caller
 */
void ToyChain_MakeDropIn9(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4,
                          uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_039BE6);
}
