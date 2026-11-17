#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_039BCA = 0x039BCAu;

/* ToyChain_MakeDropIn12B - inline chain maker of a drop-in chain: 12 figures (table $039BF0) on the
 * drop-in chain body $037B8A, as ToyChain_MakeDropIn12 with its own link table
 * In:  a6 = the holder (field $2A or $2C of the Op2_SpawnChild that names it); the inline arguments
 *      after the jsr
 * Out: d0 = 0 and the chain object, or nonzero (pool full)
 * Evidence: jsr ToyChain_SpawnInline + `000C 00039BF0 00037B8A`; Op2_SpawnChild $2A dc.l at $039B5C
 *           (script $039B5C, Goto the drop-in base body $037B12); its only namer is the script of the
 *           raw stub $039B50 (lea script / jmp Obj_SpawnEnemy), which no ROM long, table or op operand
 *           names: no reachable caller is known; no corpus run reaches it (raw-stubs 2026-10-30);
 *           ToyChain_SpawnInline pops the return address as its argument pointer and returns to this
 *           function's caller; the contract is ToyChain_MakeDropIn12's (the same call); 039BC6.syn
 *           checks it
 */
void ToyChain_MakeDropIn12B(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4, uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_039BCA);
}
