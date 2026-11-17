#include "game.h"

/* the coroutine after the native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_042498 = 0x042498u;

enum {
    HEADING_EIGHTH = 0x80,    /* $400 a turn: 45 degrees */
    HEADING_MASK = 0x3FF,
};

/* Enemy_JetTrooper_SteerEighthTurn - native at the head of a jetpack trooper coroutine 2 ($042482,
 * started by Op2_StartCoroutine2Rel at $04247C): its target heading = its current heading + an
 * eighth of a turn, wrapped, then back into the coroutine
 * In:  motion = the trooper's motion block (a3: heading +$C, target +$A)
 * Out: d0's low word = the new target; motion->heading_target = (heading + $80) & $3FF; the
 *      coroutine continues at Scr2_042498
 * Evidence: move.w $C(a3),d0 / addi.w #$80,d0 / andi.w #$3FF,d0 / move.w d0,$A(a3) / jsr $7FE.w;
 *           Op2_Native at $042482 in the trooper's subroutines after $04246C; no corpus run reaches it
 *           (raw-stubs 2026-10-30); the contract is the Op2_Native fragments' with d0 written;
 *           042484.syn checks it
 */
void Enemy_JetTrooper_SteerEighthTurn(uint32_t *d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                                      uint32_t d6, uint32_t d7, uint32_t a1, Vm2Motion *motion, uint32_t a4,
                                      uint32_t a5, uint32_t a6)
{
    uint16_t heading = (uint16_t)((uint32_t)motion->heading >> 16);
    uint16_t target = (uint16_t)((heading + HEADING_EIGHTH) & HEADING_MASK);
    *d0 = (*d0 & 0xFFFF0000u) | target;
    motion->heading_target = target;
    dk::jump_script(at_VM2_Enter, Scr2_042498);
}
