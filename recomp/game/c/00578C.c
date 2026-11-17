#include "game.h"

/* Op4_AddVelY / Op4_AddVelRadius ($00578C-$0057A7): an 8.8 word added to a velocity long. */

/* an 8.8 operand word as 16.16: ext.l / asl.l #8 */
static uint32_t word_8_8(const u16 **script)
{
    return (uint32_t)(int32_t)(int16_t)(uint16_t)*(*script)++ << 8;
}

/* Op4_AddVelY - a4 op: adds a signed 8.8 word (px/frame) to the object's y velocity.
 * In:  script = word 8.8
 * Out: d0 = it as 16.16; self->vel_y; NEXT
 * Evidence: move.w (a4)+,d0 / ext.l / asl.l #8 / add.l d0,$42(a6); the same code as Op4_AddVelYAlt
 *           ($5868); script uses $1A602, $1F13A; 5 corpus runs
 */
void Op4_AddVelY(uint32_t *d0, const u16 **script, Vm4Obj *self)
{
    *d0 = word_8_8(script);
    vm4_add_long(self->vel_y, *d0);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op4_AddVelRadius - a4 op: adds a signed 8.8 word to the object's radius velocity.
 * In:  script = word 8.8
 * Out: d0 = it as 16.16; self->vel_radius; NEXT
 * Evidence: ext.l / asl.l #8 / add.l to $36(a6); the same code as Op4_AddVelRadiusAlt ($5876);
 *           script use $1F26C; 3 corpus runs
 */
void Op4_AddVelRadius(uint32_t *d0, const u16 **script, Vm4Obj *self)
{
    *d0 = word_8_8(script);
    vm4_add_long(self->vel_radius, *d0);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
