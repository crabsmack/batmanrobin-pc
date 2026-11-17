#include "game.h"

/* the boss script after the native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_025C8A = 0x025C8Au;

/* Boss_Stage2_ReadTurretsDown - boss phase-1 loop native: the destroyed-turrets mask into script
 * variable 0, so the next op (Op2_BranchVar0Eq $F) moves to phase 2 once all four are down
 * In:  vm = the boss's a4 block; the rest the running object's registers (for the script)
 * Out: vm->var[0] = g_Stage2TurretsDown; the script continues at Scr2_025C8A (jsr $7FE.w)
 * Evidence: move.w g_Stage2TurretsDown,$22(a4) / jsr $7FE.w; followed by `204E 000F` at $025C8E;
 *           RAM: var 0 = 3 at f32700 (two turrets at energy 0), $F at f33180; TAS f32140-f33171
 */
void Boss_Stage2_ReadTurretsDown(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                                 uint32_t d7, uint32_t a1, uint32_t a3, Vm2Block *vm, uint32_t a5, uint32_t a6)
{
    vm->var[0] = g_Stage2TurretsDown;
    dk::jump_script(at_VM2_Enter, Scr2_025C8A);
}
