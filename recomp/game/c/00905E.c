#include "game.h"

/* a 68000 address held in a register, as a pointer (PTR(T): the 68000 value kept) */
template <class T> static T *ptr(uint32_t a)
{
    PTR(T) p;
    p.v = a;
    return p;
}

/* Fx_SpawnAimedN - native callback: runs an a4 spawn script at a mirrored offset, the new objects
 * heading the object's way rounded to one of N directions.
 * In:  a3 = the motion block (heading +$C, $400 a turn in its high word); a2 = the script: dc.w N,
 *      dx, dy, dc.l the a4 script
 * Out: d0 = divu's result ($40000 / N: remainder:quotient); d3 = round(heading * N / $400) *
 *      ($40000 / N) << 8 (the heading as 16.16); a2 at dx; then Fx_SpawnAtOfsWithAngle
 * Evidence: move.w (a2)+,d3 / move.l #$40000,d0 / divu.w d3,d0 / mulu.w $C(a3),d3 / addi.l #$200,d3
 *           / lsr.l #8,d3 / lsr.w #2,d3 / mulu.w d0,d3 / lsl.l #8,d3 / bra.s
 *           Fx_SpawnAtOfsWithAngle; 26 script uses at $31024-$31162. The divu overflows for N < 5
 *           (no script uses one) and then leaves d0 as it was, as the 68000 does
 */
void Fx_SpawnAimedN(uint32_t *d0, uint32_t d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t d6,
                    uint32_t d7, uint32_t a1, uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    (void)d1, (void)d2, (void)d4, (void)d5, (void)d6, (void)d7, (void)a1, (void)a4, (void)a5, (void)a6;
    const Vm2Motion *mot = ptr<const Vm2Motion>(a3);
    uint16_t n = *ptr<const u16>(*a2);
    *a2 += 2;

    uint32_t step = 0x40000u;   /* a turn ($400 heading units) x $100: the step between directions */
    uint32_t quot = step / n;
    if (quot <= 0xFFFFu)        /* divu.w: remainder in the high word, quotient in the low */
        step = (step % n) << 16 | quot;
    *d0 = step;

    uint32_t r = (uint32_t)n * (uint16_t)((uint32_t)mot->heading >> 16);   /* mulu.w $C(a3),d3 */
    r = (r + 0x200u) >> 8;                       /* round to the nearest of the N directions */
    r = (r & 0xFFFF0000u) | (uint16_t)((uint16_t)r >> 2);   /* lsr.w: the low word only */
    r = (uint32_t)(uint16_t)r * (uint16_t)step;  /* mulu.w d0,d3 */
    *d3 = r << 8;
    dk::jump(at_Fx_SpawnAtOfsWithAngle);
}
