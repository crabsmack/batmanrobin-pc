#include "game.h"

/* divs.w: the 32-bit dividend by a signed word; the remainder in the high word, the quotient in
   the low. A quotient that doesn't fit a word sets V and leaves the dividend unchanged. */
static uint32_t divs_w(uint32_t dividend, int16_t divisor)
{
    if (divisor == 0)
        dk::fatal("Mot_JumpGravityD1: divs.w by zero (a jump time of 0)");
    int64_t q = (int64_t)(int32_t)dividend / divisor;
    if (q < -32768 || q > 32767)
        return dividend;
    int32_t r = (int32_t)dividend % divisor;
    return ((uint32_t)(uint16_t)r << 16) | (uint16_t)q;
}

static uint32_t swap(uint32_t v) { return v << 16 | v >> 16; }

/* Mot_JumpGravityD1 - the gravity g of a jump of height h in time T: with the tick t = S/16,
 * g = 2h * 65536 * t^2 / (T * (T + t)), so decelerating from g*T/t to 0 over T/t runs covers h
 * pixels. Two scalings keep the divisor in a word: T + t above $800 shifts by 6 instead of 4/8.
 * In:  gravity = h (the low word); script = word time T; vm->speed (S), vm->tick_sq
 * Out: tick = S >> 4; gravity = g (long); span = (T + t) * T shifted and swapped (the divisor in
 *      its low word); time = T; script past the time word
 * Evidence: moveq #0,d0 / move.w $2E(a4),d0 / swap d1 / move.w (a2)+,d2 / asr.l #4,d0 /
 *           move.w d2,d3 / add.w d0,d2 / cmpi.w #$800,d2 / bgt.s; muls.w d3,d2 / asr.l #4 (#6),d1 /
 *           lsl.l #8 (#6),d2 / swap d2 / divs.w d2,d1 / muls.w $32(a4),d1 / asr.l #3,d1 / rts;
 *           executed in 137 corpus runs
 */
void Mot_JumpGravityD1(uint32_t *tick, uint32_t *gravity, uint32_t *span, uint16_t *time,
                       const u16 **script, const Vm2Block *vm)
{
    *tick = (uint32_t)vm->speed;
    *gravity = swap(*gravity);
    *span = (*span & 0xFFFF0000u) | *(*script)++;
    *tick >>= 4;
    *time = (uint16_t)*span;
    *span = (*span & 0xFFFF0000u) | (uint16_t)(*span + *tick);
    bool wide = (int16_t)(uint16_t)*span > 0x800;
    *span = (uint32_t)((int32_t)(int16_t)(uint16_t)*span * (int16_t)*time);
    if (!wide) {
        *gravity = (uint32_t)((int32_t)*gravity >> 4);
        *span = swap(*span << 8);
    } else {
        *gravity = (uint32_t)((int32_t)*gravity >> 6);
        *span = swap(*span << 6);
    }
    *gravity = divs_w(*gravity, (int16_t)(uint16_t)*span);
    *gravity = (uint32_t)((int32_t)(int16_t)(uint16_t)*gravity * (int16_t)vm->tick_sq);
    *gravity = (uint32_t)((int32_t)*gravity >> 3);
}
