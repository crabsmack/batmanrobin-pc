#include "game.h"

/* Op2_FxHeadingHere - a2 op: sets the effect-spawn heading d3 to the object's current heading (the
 * motion block's heading integer word, into d3's high word).
 * In:  mot (heading, 16.16)
 * Out: d3 = heading word : d3's old high word (move.w then swap); NEXT
 * Evidence: move.w $C(a3),d3 / swap d3 / NEXT; 19 corpus runs, 90 script uses; $303BA `52F0 5198 000E`
 */
void Op2_FxHeadingHere(uint32_t *d3, const u16 **script, const Vm2Motion *mot)
{
    uint16_t heading = (uint16_t)((uint32_t)(int32_t)mot->heading >> 16);
    *d3 = (uint32_t)heading << 16 | *d3 >> 16;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
