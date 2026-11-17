#include "game.h"

/* Enemy_BlackDrone_NextMuzzle - switch native in the black gun drone's animation (flying section,
 * body $027DBE): when the shot counter (script var 0) moved since the last call (var 1 keeps the
 * last value), takes it and picks the muzzle by its parity
 * In:  vm = the drone's VM block (a4: var 0 = shot requests, var 1 = the last one seen)
 * Out: d0 = 1 (even count: the lower muzzle, $10,$B) / 2 (odd: the upper, $10,-$B) for
 *      Op2_CallNativeRelSwitch, the upper word as it was; 0 (the whole long) = no new shot
 * Evidence: move.w $22(a4),d0 / cmp.w $24(a4),d0 / beq -> moveq #0,d0; else move.w d0,$24(a4) /
 *           andi.w #1,d0 / addq.w #1,d0; op $2D30 at $027E74 ... $027F4E in the body's anim; var 0
 *           bumped by Op2_IncVar0 at $027E3A; TAS f35434-f57184 (1552)
 */
void Enemy_BlackDrone_NextMuzzle(uint32_t *d0, Vm2Block *vm)
{
    uint16_t shots = vm->var[0];
    if (shots == vm->var[1]) {
        *d0 = 0;                                  /* moveq */
        return;
    }
    vm->var[1] = shots;
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)((shots & 1) + 1);   /* word ops: the upper word stays */
}
