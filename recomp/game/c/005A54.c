#include "game.h"

/* Op4_RangeVelX - a4 op: adds base + a random [0, w) (8.8 px/frame) to the object's x velocity:
 * Rand_Next's low word lsr #4 ([0, $1000)) times w, asr.l #4, plus base << 8.
 * In:  script = word base (8.8), word w
 * Out: self->vel_x; NEXT
 * Evidence: bsr Rand_Next / move.w (a4)+,d1 / ext.l / asl.l #8 / lsr.w #4,d0 / muls.w (a4)+,d0 /
 *           asr.l #4 / add.l d1,d0 / add.l d0,$3E(a6); script uses $13220 (Drop child B), $305F4;
 *           33 corpus runs
 */
void Op4_RangeVelX(const u16 **script, Vm4Obj *self)
{
    uint16_t r = (uint16_t)(uint32_t)Rand_Next();
    uint32_t base = (uint32_t)(int32_t)(int16_t)(uint16_t)*(*script)++ << 8;
    int32_t part = ((int32_t)(int16_t)(r >> 4) * (int16_t)(uint16_t)*(*script)++) >> 4;
    vm4_add_long(self->vel_x, (uint32_t)part + base);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
