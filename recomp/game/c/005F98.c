#include "game.h"

/* Op4_ChildRandAccelY - a4 op: adds a random [0, w) (8.8; w may be negative) to the last spawned
 * child's y acceleration: Rand_Next's low word lsr #4 ([0, $1000)) times w, asr.l #4.
 * In:  script = word w (8.8); child = a5
 * Out: child->acc_y; NEXT
 * Evidence: bsr Rand_Next / lsr.w #4,d0 / muls.w (a4)+,d0 / asr.l #4,d0 / add.l d0,$52(a5); script
 *           use $4955A (`5F98 FFF4`); 1 corpus run
 */
void Op4_ChildRandAccelY(const u16 **script, Vm4Obj *child)
{
    uint16_t r = (uint16_t)(uint32_t)Rand_Next();
    int32_t amount = ((int32_t)(int16_t)(r >> 4) * (int16_t)(uint16_t)*(*script)++) >> 4;
    vm4_add_long(child->acc_y, (uint32_t)amount);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
