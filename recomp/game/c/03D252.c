#include "game.h"

/* the boss's script after the native's jsr $7FE.w (VM2_Enter): Op2_StartCoroutine0Here, which
   makes Boss_Hatter_StepBounce coroutine 0 */
static constexpr uint32_t Scr2_03D260 = 0x03D260u;

enum {
    HATTER_DROP_START = 0x18,   /* the hat starts its drop 24 px down (the offset's integer part) */
};

/* Boss_Hatter_StartBounce - a2 native fragment at the end of the Hatter's defeat: starts the hat's
 * drop-and-bounce: script vars 0/1 (the bounce speed, 16.16) = 0, var 2 (the drop offset's
 * integer part) = 24 px
 * In:  vm = the boss's VM block (a4)
 * Out: vm->var[0..1] = 0, var[2] = $18; then the script at Scr2_03D260
 * Evidence: clr.l $22(a4) / move.w #$18,$26(a4) / jsr ($7FE).w; after BossMeter_Defeated,
 *           Hitbox_WipeEnemies and the explosion loops of Scr2_03D032; TAS f115654 (one frame)
 */
void Boss_Hatter_StartBounce(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                             uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, Vm2Block *vm, uint32_t a5,
                             uint32_t a6)
{
    vm->var[0] = 0;
    vm->var[1] = 0;
    vm->var[2] = HATTER_DROP_START;
    dk::jump_script(at_VM2_Enter, Scr2_03D260);
}
