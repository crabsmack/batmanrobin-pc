#include "game.h"

/* Enemy_DomeCraft_TakeFireRequest - switch native in the dome craft's animation (body $0280EC):
 * takes the shot request (bit 0 of script var 0) and clears var 0
 * In:  vm = the craft's VM block (a4: var 0 = shot requests, Op2_IncVar0 at $028164)
 * Out: d0.w = 1 when a shot was requested, else 0 (case of Op2_CallNativeRelSwitch); var 0 = 0
 * Evidence: move.w $22(a4),d0 / clr.w $22(a4) / andi.w #1,d0 / rts; op $2D30 at $0281FA and
 *           $02823A (case 1: muzzle frame + the a4 effect at $028176 by facing); RAM f41700:
 *           such an object; TAS f41668-f65089 (209)
 */
uint16_t Enemy_DomeCraft_TakeFireRequest(Vm2Block *vm)
{
    uint16_t requests = vm->var[0];
    vm->var[0] = 0;
    return (uint16_t)(requests & 1);
}
