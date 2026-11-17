#include "game.h"

/* Mot_AccelForDistance and its y-axis op, and Op2_SetCruiseY ($001B98-$001BD1). */

/* divu.w: the 32-bit dividend by an unsigned word; the remainder in the high word, the quotient
   in the low. A quotient above $FFFF sets V and leaves the dividend unchanged. */
static uint32_t divu_w(uint32_t dividend, uint16_t divisor)
{
    if (divisor == 0)
        dk::fatal("Mot_AccelForDistance: divu.w by zero (a move time of 0)");
    uint32_t q = dividend / divisor;
    if (q > 0xFFFF)
        return dividend;
    return (dividend % divisor) << 16 | q;
}

/* Mot_AccelForDistance - the acceleration for a script move: distance * speed factor S /
 * (T * (T + S/16) / 2) * tick^2, the arithmetic-series form of covering the distance in T timer
 * units. A negative distance makes the unsigned divide overflow (the dividend is kept), as on
 * the hardware.
 * In:  script = word distance, word time T; vm->speed (S), vm->tick_sq
 * Out: speed = S >> 4 (a word); accel = the acceleration (long); series = (T * (T + S/16)) with
 *      its low word halved; time = T; script past both words
 * Evidence: move.w $2E(a4),d0 / move.w (a2)+,d1 / move.w (a2)+,d2 / muls.w d0,d1 / lsr.w #4,d0 /
 *           move.w d2,d3 / add.w d0,d2 / mulu.w d3,d2 / lsr.w #1,d2 / divu.w d2,d1 /
 *           mulu.w $32(a4),d1 / rts; callers Op2_SetAccel{,X,Y}ForDist; executed in 153 corpus runs
 */
void Mot_AccelForDistance(uint16_t *speed, uint32_t *accel, uint32_t *series, uint16_t *time,
                          const u16 **script, const Vm2Block *vm)
{
    *speed = vm->speed;
    uint16_t distance = *(*script)++;
    *series = (*series & 0xFFFF0000u) | *(*script)++;
    *accel = (uint32_t)((int32_t)(int16_t)distance * (int16_t)*speed);
    *speed = (uint16_t)(*speed >> 4);
    *time = (uint16_t)*series;
    *series = (uint32_t)(uint16_t)(*series + *speed) * *time;
    *series = (*series & 0xFFFF0000u) | (uint16_t)((uint16_t)*series >> 1);
    *accel = divu_w(*accel, (uint16_t)*series);
    *accel = (uint32_t)(uint16_t)*accel * vm->tick_sq;
}

/* Op2_SetAccelYForDist - a2 op: the y acceleration from a distance and a time
 * (Mot_AccelForDistance).
 * In:  script = word distance, word time
 * Out: the registers as Mot_AccelForDistance leaves them; mot->accel_y; handler = the next op's;
 *      NEXT
 * Evidence: bsr.s Mot_AccelForDistance / move.l d1,$28(a3) / NEXT; 118 corpus runs incl. the TAS
 */
void Op2_SetAccelYForDist(uint16_t *speed, uint32_t *accel, uint32_t *series, uint16_t *time,
                          uint32_t *handler, const u16 **script, Vm2Motion *mot, const Vm2Block *vm)
{
    Mot_AccelForDistance(speed, accel, series, time, script, vm);
    mot->accel_y = (int32_t)*accel;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_SetCruiseY - a2 op: the y cruise speed (also the fall speed limit of the jump ops) = word *
 * the speed factor (unsigned multiply).
 * In:  script = word speed; vm->speed
 * Out: cruise = word * speed (mulu.w; the high word of the register is replaced); mot->cruise_y;
 *      NEXT
 * Evidence: move.w (a2)+,d0 / mulu.w $2E(a4),d0 / move.l d0,$34(a3) / NEXT; 146 corpus runs incl.
 *           the TAS
 */
void Op2_SetCruiseY(uint32_t *cruise, const u16 **script, Vm2Motion *mot, const Vm2Block *vm)
{
    *cruise = (uint32_t)*(*script)++ * vm->speed;
    mot->cruise_y = (int32_t)*cruise;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
