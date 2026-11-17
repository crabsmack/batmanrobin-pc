#include "game.h"

/* the drone's a2 script (ROM, right after this spawner's code) */
static constexpr uint32_t Scr2_01F772 = 0x01F772u;

/* Enemy_Copter_SpawnGreen - wave-script spawner of the slower green propeller drone (Scr2_01F772: energy $40, cruise $2EA), the delay word as
 * Enemy_Copter_SpawnRed
 * In:  d4/d5 = x/y; a1 = the wave script's operand (the delay word)
 * Out: the drone's VM block (a4): resume = Scr2_01F772, loop counter 0 = the delay; a1 past the word;
 *      Z set. When the pool is short Obj_SpawnScriptedOrQuit returns to this spawner's caller itself
 *      (the rest never runs, as in the asm)
 * Evidence: jsr Obj_SpawnScriptedOrQuit / move.l #$1F772,$2A(a4) / move.w (a1)+,$14(a4) /
 *           moveq #0,d0 / rts; Scr2_01F772's cruise $2EA; the same shape as Enemy_Copter_SpawnRed
 */
void Enemy_Copter_SpawnGreen(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a1, uint32_t *a3, uint32_t *a4,
                             uint32_t *a6)
{
    uint32_t d0 = 0, a0 = 0, a5 = 0;  /* clobbered (moveq #0,d0 at the end: no caller reads d0) */
    Obj_SpawnScriptedOrQuit(&d0, d1, d4, d5, d7, &a0, a3, a4, &a5, a6);
    PTR(Vm2Block) block;
    block.v = *a4;
    Vm2Block *vm = block;
    vm->resume.v = Scr2_01F772;
    PTR(const u16) operand;
    operand.v = *a1;
    vm->counter[0] = *operand;
    *a1 += 2;
    /* moveq #0,d0: Z set, N/V/C clear, as Obj_SpawnScriptedOrQuit's own moveq #0 left them */
}
