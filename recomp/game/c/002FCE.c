#include "game.h"

/* Op2_SetRandJitter ($002FCE-$002FDF). */

/* Op2_SetRandJitter - a2 op: sets the object's random offset to a value in [-w, w) (the signed
 * random word times w, doubled, high word), so enemies sharing a script keep slightly different
 * distances (added to the windows of Op2_HoldRangeOfPlayerX* and Op2_HoldPlayerWindow).
 * In:  script = word w
 * Out: self->jitter; d0 = the doubled product with its halves swapped; NEXT
 * Evidence: bsr.w Rand_Next / muls.w (a2)+,d0 / add.l d0,d0 / swap d0 / move.w d0,$1C(a6) / NEXT;
 *           26 runs; $1BF76 `2FCE 0020`
 */
void Op2_SetRandJitter(uint32_t *d0, const u16 **script, Vm2Obj *self)
{
    int16_t r = (int16_t)(uint16_t)Rand_Next();
    int16_t w = (int16_t)(uint16_t)*(*script)++;
    uint32_t doubled = (uint32_t)((int32_t)r * w) * 2u;
    *d0 = (doubled << 16) | (doubled >> 16);   /* swap */
    self->jitter = (int16_t)(uint16_t)*d0;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
