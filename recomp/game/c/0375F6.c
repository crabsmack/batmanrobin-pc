#include "game.h"

/* the ring body's script after the native (the bytes after its jsr $7FE.w): the ring loop */
static constexpr uint32_t Scr2_037604 = 0x037604u;

/* ToyRing_DoubleKnockback - native fragment of the toy-ring body (Scr2_037588): after Op2_TakeHit
 * it doubles the x impulse, so a hit ring is thrown back twice as far, then continues the script
 * In:  motion = the ring's motion block (a3: +2 the x impulse, 16.16); the rest the object's
 *      registers (for the script)
 * Out: motion x impulse *= 2; the script continues at Scr2_037604 (jsr $7FE.w)
 * Evidence: move.l 2(a3),d0 / add.l d0,d0 / move.l d0,2(a3) / jsr ($7FE).w; reached after Op2_TakeHit
 *           at $0375F0; TAS f79739-f112908 (57 frames). d0 is no output of the measured contract
 *           (the script never reads it)
 */
void ToyRing_DoubleKnockback(uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                             uint32_t d7, uint32_t a1, Vm2Motion *motion, uint32_t a4, uint32_t a5, uint32_t a6)
{
    motion->impulse_x = (int32_t)((uint32_t)motion->impulse_x * 2u);
    dk::jump_script(at_VM2_Enter, Scr2_037604);
}
