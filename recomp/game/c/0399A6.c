#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_0399AA = 0x0399AAu;

/* ToyChain_MakeChild8B - inline chain maker of ToyChain_Spawn8B: an 8-figure chain (table
 * Tbl_039A0A+$BA, as ToyChain_MakeChild7B's), fixed speed $100, sway cruise $255, brake distance $50
 * In:  a6 = the base (ToyChain_Spawn8B, field $2A); inline dc.w 8, dc.l $039AC4, dc.l $0399B4
 * Out: d0 = 0 and the chain object, or nonzero (pool full)
 * Evidence: jsr ToyChain_SpawnInline + `0008 00039AC4 000399B4`; Op2_SpawnChild 002A at $0398A6 (the script
 *           of ToyChain_Spawn8B); no corpus run calls it at 4:3; widescreen_gameplay=1 may (ws-tail
 *           2026-10-30); 0399A6.syn checks it; ToyChain_SpawnInline pops the return address as its
 *           argument pointer and returns to this function's caller
 */
void ToyChain_MakeChild8B(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4,
                          uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_0399AA);
}
