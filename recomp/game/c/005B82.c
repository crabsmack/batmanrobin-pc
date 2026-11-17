#include "game.h"

/* Op4_JitterAccelX - a4 op: adds a random [-w/2, w/2) (8.8) to the object's x acceleration:
 * Rand_Next's low word asr #4 times w, asr.l #4.
 * In:  script = word w
 * Out: d0 = the amount (16.16); self->acc_x; NEXT
 * Evidence: bsr Rand_Next / asr.w #4,d0 / muls.w (a4)+,d0 / asr.l #4 / add.l d0,$4E(a6); script uses
 *           $35A5A, $3B72E, $46968; 5 corpus runs
 */
void Op4_JitterAccelX(uint32_t *d0, const u16 **script, Vm4Obj *self)
{
    int16_t r = (int16_t)(uint16_t)(uint32_t)Rand_Next();
    *d0 = (uint32_t)(((int32_t)(int16_t)(r >> 4) * (int16_t)(uint16_t)*(*script)++) >> 4);
    vm4_add_long(self->acc_x, *d0);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
