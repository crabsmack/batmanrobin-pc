#include "game.h"

/* Op5_Wait / Op5_WaitRandom ($006AC4-$006AE5): the wave script waits: the timer is set (time
 * words are 1/16 frame: x 16 against g_WaveTimerStep's $100 per frame) and the script yields. */

/* Op5_Wait - a5 op: waits t/16 frames (timer = t * 16, counted down only while the population is
 * below the cap), then continues.
 * In:  script = word t; a6 = the wave object
 * Out: d0 = t * 16; self->timer; the move.l's condition codes; VM5_Yield
 * Evidence: moveq #0,d0 / move.w (a5)+,d0 / asl.l #4,d0 / move.l d0,$2E(a6) / bra.s VM5_Yield; 858
 *           script uses, e.g. $16946 (`6AC4 00F0`), $16956 (`6AC4 0780`); 104 corpus runs
 */
void Op5_Wait(uint32_t *d0, const u16 **script, Vm5Wave *self, dk::Flags *ccr)
{
    *d0 = (uint32_t)(uint16_t)*(*script)++ << 4;
    self->timer = (int32_t)*d0;
    *ccr = dk::Flags::test(*d0, 4);
    dk::jump(at_VM5_Yield);
}

/* Op5_WaitRandom - a5 op: waits base + a random number below range (both 1/16 frames).
 * In:  script = word range, word base; a6 = the wave object
 * Out: d1 = base * 16; d0 = the wait * 16; self->timer; the move.l's condition codes; VM5_Yield
 * Evidence: move.w (a5)+,d0 / bsr.w Rand_Below / asl.l #4,d0 / moveq #0,d1 / move.w (a5)+,d1 /
 *           lsl.l #4,d1 / add.l d1,d0 / move.l d0,$2E(a6); script uses $3433A (`6AD0 0B40 0B40`),
 *           $350A4; 3 corpus runs
 */
void Op5_WaitRandom(uint32_t *d0, uint32_t *d1, const u16 **script, Vm5Wave *self, dk::Flags *ccr)
{
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)*(*script)++;
    Rand_Below(d0);
    *d0 <<= 4;
    *d1 = (uint32_t)(uint16_t)*(*script)++ << 4;
    *d0 += *d1;
    self->timer = (int32_t)*d0;
    *ccr = dk::Flags::test(*d0, 4);
    dk::jump(at_VM5_Yield);
}
