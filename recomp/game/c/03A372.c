#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_03A376 = 0x03A376u;

/* ToyChain_MakeBox10D - inline chain maker of a gift box's 10-figure chain (table $03A52E) on the
 * swaying body $037DD6 (oscillator X $40/$55/$55/$78, as ToyChain_MakeBox10A's)
 * In:  a6 = the holder (field $2A or $2C of the Op2_SpawnChild that names it); the inline arguments
 *      after the jsr
 * Out: d0 = 0 and the chain object, or nonzero (pool full)
 * Evidence: jsr ToyChain_SpawnInline + `000A 0003A52E 0003A380`; Op2_SpawnChild $2A dc.l at $03A292
 *           (script $03A270); its only namer is the script of the raw stub $03A264 (lea script / jmp
 *           Obj_SpawnEnemy), which no ROM long, table or op operand names: no reachable caller is known
 *           (the long at $084DE6 is art); no corpus run reaches it (raw-stubs 2026-10-30);
 *           ToyChain_SpawnInline pops the return address as its argument pointer and returns to this
 *           function's caller; the contract is ToyChain_MakeBox10A's (the same call); 03A372.syn checks
 *           it
 */
void ToyChain_MakeBox10D(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4, uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_03A376);
}
