#include "game.h"

/* Boss_Stage2_RampExplosions - defeated boss's coroutine 0 native while no enemy is left and the
 * end timer runs: raises the explosion rate (variable 3) by $80 a frame, saturating at $FFFF,
 * then ticks the explosions
 * In:  vm = the boss's a4 block; the rest the running object's registers (for the script)
 * Out: vm->var[3] += $80 (at most $FFFF); then Boss_Stage2_TickExplosions
 * Evidence: addi.w #$80,$28(a4) / bcc Boss_Stage2_TickExplosions / move.w #$FFFF,$28(a4) / bra;
 *           var 3 starts at $800 (Op2_SetVar3 at $025D54); TAS f33964-f34682 (719 frames)
 */
void Boss_Stage2_RampExplosions(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                                uint32_t d7, uint32_t a1, uint32_t a3, Vm2Block *vm, uint32_t a5, uint32_t a6)
{
    uint32_t rate = (uint32_t)vm->var[3] + 0x80;
    vm->var[3] = rate > 0xFFFFu ? 0xFFFF : (uint16_t)rate;   /* bcc: no carry keeps the sum */
    dk::jump(at_Boss_Stage2_TickExplosions);   /* both paths branch: falls into the next file */
}
