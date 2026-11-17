#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_039A6A = 0x039A6Au;

/* ToyChain_MakeChild7C - inline chain maker of ToyChain_Spawn7C: a 7-figure chain (table
 * Tbl_039A0A+$D6), random speed $4C-$100 (op $0AE6), sway cruise $155, brake distance $78
 * In:  a6 = the base (ToyChain_Spawn7C, field $2A); inline dc.w 7, dc.l $039AE0, dc.l $039A74
 * Out: d0 = 0 and the chain object, or nonzero (pool full)
 * Evidence: jsr ToyChain_SpawnInline + `0007 00039AE0 00039A74`; Op2_SpawnChild 002A at $039786 (the script
 *           of ToyChain_Spawn7C); no corpus run calls it at 4:3; widescreen_gameplay=1 may (ws-tail
 *           2026-10-30); 039A66.syn checks it; ToyChain_SpawnInline pops the return address as its
 *           argument pointer and returns to this function's caller
 */
void ToyChain_MakeChild7C(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4,
                          uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_039A6A);
}
