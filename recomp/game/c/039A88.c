#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_039A8C = 0x039A8Cu;

/* ToyChain_MakeChild5C - inline chain maker of a toy-chain base: a 5-figure chain (table $039B34) at
 * speed $100 with a fast sway (cruise $355, brake distance $78) on Scr2_0379DA
 * In:  a6 = the holder (field $2A or $2C of the Op2_SpawnChild that names it); the inline arguments
 *      after the jsr
 * Out: d0 = 0 and the chain object, or nonzero (pool full)
 * Evidence: jsr ToyChain_SpawnInline + `0005 00039B34 00039A96`; Op2_SpawnChild $2A dc.l at $03975E
 *           (script $03975E); its only namer is the script of the raw stub $039752 (lea script / jmp
 *           Obj_SpawnEnemy), which no ROM long, table or op operand names: no reachable caller is
 *           known; no corpus run reaches it (raw-stubs 2026-10-30); ToyChain_SpawnInline pops the
 *           return address as its argument pointer and returns to this function's caller; the contract
 *           is ToyChain_MakeChild10's (the same call); 039A88.syn checks it
 */
void ToyChain_MakeChild5C(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4, uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_039A8C);
}
