#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_03AEEE = 0x03AEEEu;

/* ToyChain_MakeHanging8 - chain-child spawner for Op2_SpawnChild: ToyChain_SpawnInline with 8
 * links, parameter table $03B174 (link energy $60, touch callback EnemyAtk_HitKnockSpeed $10/$200)
 * and script $03AEF8 (random speed, sway oscillator, charge cycle $038810, coroutine 0 $03822C,
 * chain body $03802E)
 * In:  d4/d5 = position (from Op2_SpawnChild)
 * Out: d0 = 0 / a0 = chain block, or nonzero when the pool is full
 * Evidence: jsr ToyChain_SpawnInline(pc) / dc.w 8 / dc.l $03B174 / dc.l $03AEF8; dc.l $03AEEA in
 *           Scr2_03A9B4+8 and Scr2_03AA26+8; TAS f96963-f97271; RAM f97380: the chain object (owner
 *           $B) beside its propeller toy; ToyChain_SpawnInline pops the return address as its
 *           argument pointer and returns to this function's caller
 */
void ToyChain_MakeHanging8(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4, uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_03AEEE);
}
