#include "game.h"

/* Stage4_TreadFrameByX - the 3-frame tread animation phase of a tracked machine: (x / 2) mod 3, so
 * the treads turn as the object moves
 * In:  obj = the object (a6: +$3E x)
 * Out: d0 = the quotient in the high word (divu's, swapped) and the phase 0-2 in the low word
 * Evidence: moveq #0,d0 / move.w $3E(a6),d0 / lsr.w #1 / divu.w #3 / swap d0; Op2_CallNativeRelSwitch
 *           at $42E96 (vent tank: DrawSpr $4B32/$4B28/$4B1E) and $430D0 (gun tank: DrawSprOff
 *           $4BB4/$4BBE/$4BC8); 1964 TAS frames 119574-122361 (stage 4 lab)
 */
uint32_t Stage4_TreadFrameByX(const Vm2Obj *obj)
{
    uint32_t half = (uint16_t)obj->x >> 1;   /* <= $7FFF: the divu never overflows */
    uint32_t quotient = half / 3, phase = half % 3;
    return (quotient << 16) | phase;
}
