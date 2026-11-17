#include "game.h"

/* Op4_ChildRangeAccelY - a4 op: adds base + a random [0, w) (8.8) to the last spawned child's y
 * acceleration (the gravity spread of debris).
 * In:  script = word base (8.8), word w (8.8); child = a5
 * Out: d1 = base (16.16); child->acc_y; NEXT
 * Evidence: bsr Rand_Next / move.w (a4)+,d1 / ext.l / asl.l #8 / lsr.w #4,d0 / muls.w (a4)+,d0 /
 *           asr.l #4 / add.l d1,d0 / add.l d0,$52(a5); script uses $35824 (`5FAA 0026 0026`),
 *           $36F92; 37 corpus runs
 */
void Op4_ChildRangeAccelY(uint32_t *d1, const u16 **script, Vm4Obj *child)
{
    uint16_t r = (uint16_t)(uint32_t)Rand_Next();
    *d1 = (uint32_t)(int32_t)(int16_t)(uint16_t)*(*script)++ << 8;
    int32_t part = ((int32_t)(int16_t)(r >> 4) * (int16_t)(uint16_t)*(*script)++) >> 4;
    vm4_add_long(child->acc_y, (uint32_t)part + *d1);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
