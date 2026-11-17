#include "game.h"

/* The second add family ($00583C-$005883): a signed pixel word added to the integer word of the
 * object's y, radius or heading, and 8.8 words added to its velocities. */

/* the next handler word (NEXT) */
static uint32_t next_op(const u16 **script)
{
    return dk::code((int16_t)(uint16_t)*(*script)++);
}

/* an 8.8 operand word as 16.16: ext.l / asl.l #8 */
static uint32_t word_8_8(const u16 **script)
{
    return (uint32_t)(int32_t)(int16_t)(uint16_t)*(*script)++ << 8;
}

/* Op4_AddY - a4 op: adds a signed pixel word to the integer word of the object's y.
 * In:  script = word px
 * Out: d0 = it; self->y; NEXT (handler = the next op)
 * Evidence: move.w (a4)+,d0 / add.w d0,$32(a6) / NEXT; script uses $13252 (-$14 in Drop child A),
 *           $20E6C, $46952; 26 corpus runs
 */
void Op4_AddY(uint16_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *self)
{
    *d0 = *(*script)++;
    self->y = (int16_t)(uint16_t)((uint16_t)self->y + *d0);
    *handler = next_op(script);
    dk::jump(*handler);
}

/* Op4_AddRadius - a4 op: adds a signed word to the integer word of the object's radius.
 * In:  script = word px
 * Out: d0 = it; self->radius; NEXT
 * Evidence: move.w (a4)+,d0 / add.w d0,$26(a6); the radius scales the (cos, sin)(heading) offset
 *           of the drawn point; 21 corpus runs
 */
void Op4_AddRadius(uint16_t *d0, const u16 **script, Vm4Obj *self)
{
    *d0 = *(*script)++;
    self->radius = (int16_t)(uint16_t)((uint16_t)self->radius + *d0);
    dk::jump(next_op(script));
}

/* Op4_AddHeading - a4 op: adds a signed word to the integer word of the object's heading ($400 a
 * turn).
 * In:  script = word amount
 * Out: self->heading; NEXT
 * Evidence: move.w (a4)+,d0 / add.w d0,$2A(a6); script uses $26AAC, $29816, $36022; 27 corpus runs
 */
void Op4_AddHeading(const u16 **script, Vm4Obj *self)
{
    uint16_t amount = *(*script)++;
    self->heading = (int16_t)(uint16_t)((uint16_t)self->heading + amount);
    dk::jump(next_op(script));
}

/* Op4_AddVelX - a4 op: adds a signed 8.8 word (px/frame) to the object's x velocity.
 * In:  script = word 8.8
 * Out: d0 = it as 16.16; self->vel_x; NEXT
 * Evidence: move.w (a4)+,d0 / ext.l / asl.l #8 / add.l d0,$3E(a6); 26 corpus runs
 */
void Op4_AddVelX(uint32_t *d0, const u16 **script, Vm4Obj *self)
{
    *d0 = word_8_8(script);
    vm4_add_long(self->vel_x, *d0);
    dk::jump(next_op(script));
}

/* Op4_AddVelYAlt - a4 op: the second copy of the y-velocity add (Op4_AddVelY's code).
 * In:  script = word 8.8
 * Out: d0 = it as 16.16; self->vel_y; NEXT
 * Evidence: byte for byte the code of Op4_AddVelY ($578C); script uses $26D4E (`5868 0200`), $26D7C;
 *           3 corpus runs (fuzz only)
 */
void Op4_AddVelYAlt(uint32_t *d0, const u16 **script, Vm4Obj *self)
{
    *d0 = word_8_8(script);
    vm4_add_long(self->vel_y, *d0);
    dk::jump(next_op(script));
}

/* Op4_AddVelRadiusAlt - a4 op: the second copy of the radius-velocity add (Op4_AddVelRadius's code).
 * In:  script = word 8.8
 * Out: d0 = it as 16.16; self->vel_radius; NEXT
 * Evidence: the same code as Op4_AddVelRadius ($579A); script uses $29812, $298DE, $298F8; 38 runs
 */
void Op4_AddVelRadiusAlt(uint32_t *d0, const u16 **script, Vm4Obj *self)
{
    *d0 = word_8_8(script);
    vm4_add_long(self->vel_radius, *d0);
    dk::jump(next_op(script));
}
