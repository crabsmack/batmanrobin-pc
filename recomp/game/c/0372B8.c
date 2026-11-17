#include "game.h"

/* ToyChain_RemoveAllLinks - removes every link of a chain (ToyChain_RemoveLastLink until it reports
 * none left)
 * In:  vm = the chain's VM block (a4: +$24 the link count); self = the chain object (a6)
 * Out: the link count 0; d0 = 1; d2 / a1 as the last removal left them
 * Evidence: bsr ToyChain_RemoveLastLink / beq.s ToyChain_RemoveAllLinks / rts; called by
 *           ToyChain_KillChild and the chain death path $037A64
 */
void ToyChain_RemoveAllLinks(uint32_t *d0, uint16_t *d2, uint32_t *a1, Vm2Block *vm, Vm2Obj *self)
{
    do
        ToyChain_RemoveLastLink(d0, d2, a1, vm, self);
    while (*d0 == 0);   /* beq: its Z is d0 == 0 (tst) */
}
