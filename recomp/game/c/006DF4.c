#include "game.h"

/* Op5_SetOrigin / Op5_SetOriginY / Op5_SetOriginX ($006DF4-$006E07): the wave's spawn origin
 * offset, added to every spawn entry's position. */

/* Op5_SetOrigin - a5 op: sets the spawn origin offset (x, y words).
 * In:  script = word x, word y; a6 = the wave
 * Out: self->origin_x; then as Op5_SetOriginY
 * Evidence: move.w (a5)+,$26(a6), falling into Op5_SetOriginY; script uses $23CFC (`6DF4 0154
 *           0094`), $23F02; 9 corpus runs
 */
void Op5_SetOrigin(const u16 **script, Vm5Wave *self)
{
    self->origin_x = (int16_t)(uint16_t)*(*script)++;
    Op5_SetOriginY(script, self);
}

/* Op5_SetOriginY - a5 op: sets the spawn origin y offset.
 * In:  script = word y; a6 = the wave
 * Out: self->origin_y; NEXT
 * Evidence: move.w (a5)+,$28(a6) / NEXT; an entry inside Op5_SetOrigin (computed-jump target in 4
 *           corpus runs); script uses $228AE (`6DF8 0114`), $22BAA
 */
void Op5_SetOriginY(const u16 **script, Vm5Wave *self)
{
    self->origin_y = (int16_t)(uint16_t)*(*script)++;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op5_SetOriginX - a5 op: sets the spawn origin x offset.
 * In:  script = word x; a6 = the wave
 * Out: self->origin_x; a0 = the handler; NEXT
 * Evidence: move.w (a5)+,$26(a6) / NEXT; script uses $22108 (`6E00 0154`), $222B6; 9 corpus runs
 */
void Op5_SetOriginX(uint32_t *handler, const u16 **script, Vm5Wave *self)
{
    self->origin_x = (int16_t)(uint16_t)*(*script)++;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
