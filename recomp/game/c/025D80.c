#include "game.h"

/* the boss coroutine's yield after the native (the bytes after its jsr $7FE.w: Op2_Return) */
static constexpr uint32_t Scr2_025D8E = 0x025D8Eu;

/* Boss_Stage2_TickExplosions - defeated boss's coroutine 0 native: adds the explosion rate
 * (variable 3) to the accumulator (variable 2); when it carries, an explosion is due
 * In:  vm = the boss's a4 block; the rest the running object's registers (for the script)
 * Out: d0.w = the rate; vm->var[2] += rate; a carry continues at Boss_Stage2_ExplosionDue, else
 *      the script yields at Scr2_025D8E (jsr $7FE.w)
 * Evidence: move.w $28(a4),d0 / add.w d0,$26(a4) / bcs Boss_Stage2_ExplosionDue / jsr $7FE.w;
 *           runs alone (no ramp) while enemies remain; TAS f33605-f34682 (1078 frames). The
 *           prototype takes &d0 (measured an input: the contract ends at the jsr) so the script
 *           gets the rate in d0 as on the 68000
 */
void Boss_Stage2_TickExplosions(uint32_t *d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                                uint32_t d7, uint32_t a1, uint32_t a3, Vm2Block *vm, uint32_t a5, uint32_t a6)
{
    uint16_t rate = vm->var[3];
    *d0 = (*d0 & 0xFFFF0000u) | rate;   /* move.w: the upper word stays */
    uint32_t sum = (uint32_t)vm->var[2] + rate;
    vm->var[2] = (uint16_t)sum;
    if (sum > 0xFFFFu) {   /* bcs */
        dk::jump(at_Boss_Stage2_ExplosionDue);
        return;
    }
    dk::jump_script(at_VM2_Enter, Scr2_025D8E);
}
