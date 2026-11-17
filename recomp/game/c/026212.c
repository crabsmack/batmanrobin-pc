#include "game.h"

/* the turret script after the native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_02621E = 0x02621Eu;

/* Boss_Stage2_TurretDestroyed - turret script native when its energy runs out: ORs the turret's
 * bit (script variable 0) into the boss's destroyed-turrets mask, then the score, explosion and
 * wreck animation
 * In:  vm = the turret's a4 block (var 0 = 1/2/4/8); the rest the running object's registers
 * Out: d0.w = the bit; g_Stage2TurretsDown |= bit; the script continues at Scr2_02621E
 *      (jsr $7FE.w)
 * Evidence: move.w $22(a4),d0 / or.w d0,g_Stage2TurretsDown / jsr $7FE.w; reached by
 *           Op2_TakeDamage's branch at $0261F4; TAS f32655-f33170 (4 runs, one per turret). The
 *           prototype takes &d0 (measured an input: the contract ends at the jsr) so the script
 *           gets d0 as on the 68000
 */
void Boss_Stage2_TurretDestroyed(uint32_t *d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                                 uint32_t d7, uint32_t a1, uint32_t a3, const Vm2Block *vm, uint32_t a5, uint32_t a6)
{
    uint16_t bit = vm->var[0];
    *d0 = (*d0 & 0xFFFF0000u) | bit;   /* move.w: the upper word stays */
    g_Stage2TurretsDown = (uint16_t)(g_Stage2TurretsDown | bit);
    dk::jump_script(at_VM2_Enter, Scr2_02621E);
}
