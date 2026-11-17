#include "game.h"

/* Enemy_BombRabbit_TickFuse - fuse tick of a lit bomb rabbit: script variable 0 (+$22) += the
 * inline rate, set to $8000 once it goes negative; variable 1 (+$24) += variable 0, so variable
 * 1's top bit (the bomb's blink) toggles faster and faster
 * In:  script = the script after the native (a2: word rate, $140); vm = the rabbit's a4 block
 * Out: d0 = the new variable 0; script past the rate word
 * Evidence: move.w $22(a4),d0 / add.w (a2)+,d0 / bpl / move.w #$8000,d0 / move.w d0,$22(a4) /
 *           add.w d0,$24(a4); `2CB2 007C 0140` at $35E7E every frame of the fuse coroutine; TAS
 *           f82646-f115353. The header's "var 1/2" counts from 1: the struct's var[0]/var[1]
 */
void Enemy_BombRabbit_TickFuse(uint16_t *d0, const u16 **script, Vm2Block *vm)
{
    uint16_t rate = **script;
    ++*script;
    uint16_t v = (uint16_t)(vm->var[0] + rate);
    if ((int16_t)v < 0)
        v = 0x8000;
    *d0 = v;
    vm->var[0] = v;
    vm->var[1] = (uint16_t)(vm->var[1] + v);
}

/* Enemy_BombRabbit_FuseBlink - animation switch native: the top bit of script variable 1 (+$24),
 * picking which of the two bomb frames ($4718 / $4722) the rabbit's animation draws (the blink
 * Enemy_BombRabbit_TickFuse drives)
 * In:  vm = the rabbit's a4 block
 * Out: d0's low word = 0 or 1
 * Evidence: move.w $24(a4),d0 / rol.w #1,d0 / andi.w #1,d0 / rts; `2D30 FFxx` cases in the rabbit
 *           animation scripts $35F1E-$36012; a computed-call target in the corpus
 */
uint16_t Enemy_BombRabbit_FuseBlink(const Vm2Block *vm)
{
    return (uint16_t)((vm->var[1] >> 15) & 1u);
}
