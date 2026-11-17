#include "game.h"

/* Op4_AddAccelY - a4 op: adds a signed 8.8 word to the object's y acceleration.
 * In:  script = word 8.8
 * Out: d0 = it as 16.16; self->acc_y; NEXT
 * Evidence: move.w (a4)+,d0 / ext.l / asl.l #8 / add.l d0,$52(a6); script use $3B732; 3 corpus runs
 */
void Op4_AddAccelY(uint32_t *d0, const u16 **script, Vm4Obj *self)
{
    *d0 = (uint32_t)(int32_t)(int16_t)(uint16_t)*(*script)++ << 8;
    vm4_add_long(self->acc_y, *d0);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
