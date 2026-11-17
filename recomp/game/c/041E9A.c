#include "game.h"

/* the orb's frame loop after the native's jsr $7FE.w (VM2_Enter) */
static constexpr uint32_t Scr2_041EA4 = 0x041EA4u;

/* Enemy_GasOrb_SpinJets - a2 native fragment in the gas orb's body: turns the heading of its jets'
 * motion block by twice that block's x move this frame, so the three jets (120 degrees apart
 * around that heading) rotate as the orb moves sideways; then the script goes on
 * In:  d1 = the block's x move (16.16, Op2_StepVelocity's); jets = the motion block at $E(a6)
 *      (a3, Op2_LoadA3Handle)
 * Out: jets->heading += 2 x d1; d1 = 2 x the move; then the script at Scr2_041EA4
 * Evidence: add.l d1,d1 / add.l d1,$C(a3) / jsr ($7FE).w; after Op2_LoadA3Handle $E and
 *           Op2_StepVelocity in the body $041E4A; TAS f124189-f130140. d1 an output (measured: an
 *           input): the script goes on with the registers the native leaves
 */
void Enemy_GasOrb_SpinJets(uint32_t d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                           uint32_t d7, uint32_t a1, Vm2Motion *jets, uint32_t a4, uint32_t a5, uint32_t a6)
{
    *d1 += *d1;
    jets->heading = (int32_t)((uint32_t)jets->heading + *d1);
    dk::jump_script(at_VM2_Enter, Scr2_041EA4);
}
