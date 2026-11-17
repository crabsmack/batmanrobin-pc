#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_0399CA = 0x0399CAu;

/* ToyChain_MakeChild10B - inline chain maker of a toy-chain base: a 10-figure chain (table $039AA8) at
 * speed $100 with sway cruise $100 on the chain body Scr2_0379DA
 * In:  a6 = the holder (field $2A or $2C of the Op2_SpawnChild that names it); the inline arguments
 *      after the jsr
 * Out: d0 = 0 and the chain object, or nonzero (pool full)
 * Evidence: jsr ToyChain_SpawnInline + `000A 00039AA8 000399D4`; Op2_SpawnChild $2A dc.l at $039686
 *           (script $039686); its only namer is the script of the raw stub $03967A (lea script / jmp
 *           Obj_SpawnEnemy), which no ROM long, table or op operand names: no reachable caller is
 *           known; no corpus run reaches it (raw-stubs 2026-10-30); ToyChain_SpawnInline pops the
 *           return address as its argument pointer and returns to this function's caller; the contract
 *           is ToyChain_MakeChild10's (the same call); 0399C6.syn checks it
 */
void ToyChain_MakeChild10B(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4, uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_0399CA);
}
