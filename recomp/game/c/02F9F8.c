#include "game.h"

/* the a2 script after the jsr $7FE.w: Op2_TurnHeading toward the new target, then Op2_Return
   (batmanrobin.asm) */
static constexpr uint32_t Scr2_02FA0C = 0x02FA0Cu;

enum {
    SPIN_STEP = 0x80,       /* 45 degrees clockwise (a full turn is $400) */
    SPIN_HEADING_MASK = 0x3FF,
};

/* Enemy_HullTurret_SpinStep - native coroutine step of the spinning hull turret: the target
 * heading = the current heading + $80 (45 degrees clockwise), then back into the script, which
 * turns toward it (Op2_TurnHeading) and returns
 * In:  motion = the turret's motion block (a3); the other registers as the coroutine left them
 * Out: motion->heading_target; then VM2_Enter on Scr2_02FA0C with every register as it came (d0,
 *      the new heading in the asm, is rewritten by Op2_TurnHeading's Mot_TurnTowardHeading before
 *      anything reads it)
 * Evidence: move.w $C(a3),d0 / addi.w #$80,d0 / andi.w #$3FF,d0 / move.w d0,$A(a3) / jsr ($7FE).w;
 *           Op2_Native $07FC at $2F9F6, the coroutine-0 target ($31252, $311E2, $312B6, $31378) of
 *           the spinning turret spawners; TAS 68275-73904 (108 frames)
 */
void Enemy_HullTurret_SpinStep(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    PTR(Vm2Motion) motion;
    motion.v = a3;
    uint16_t heading = (uint16_t)((uint32_t)motion->heading >> 16);   /* its integer word */
    motion->heading_target = (uint16_t)((heading + SPIN_STEP) & SPIN_HEADING_MASK);
    dk::jump_script(at_VM2_Enter, Scr2_02FA0C);
}
