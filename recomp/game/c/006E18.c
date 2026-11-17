#include "game.h"

/* Op5_AddOriginX / Op5_OriginAtScreenCentre ($006E18-$006E3D): moving the wave's spawn origin. */

/* Op5_AddOriginX - a5 op: adds a signed word to the spawn origin x offset (spawns marching along
 * a row).
 * In:  script = word dx; a6 = the wave
 * Out: d0 = dx; self->origin_x; NEXT
 * Evidence: move.w (a5)+,d0 / add.w d0,$26(a6) / NEXT; script uses $3C4B6 (`6E18 001E 6CB0 FFF2`:
 *           +30 px per loop pass), $3C4D6 (`FFE2`); 3 corpus runs
 */
void Op5_AddOriginX(uint16_t *d0, const u16 **script, Vm5Wave *self)
{
    *d0 = *(*script)++;
    self->origin_x = (int16_t)(uint16_t)((uint16_t)self->origin_x + *d0);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op5_OriginAtScreenCentre - a5 op: sets the spawn origin offset to the screen centre in world
 * coordinates (g_CameraX + $A0, g_CameraY + $70).
 * In:  script (no operands); a6 = the wave
 * Out: d0 = the y; self->origin_x / origin_y; a0 = the handler; NEXT
 * Evidence: move.w (g_CameraX).w,d0 / addi.w #$A0,d0 / move.w d0,$26(a6) / move.w (g_CameraY).w,d0 /
 *           addi.w #$70,d0 / move.w d0,$28(a6); script uses $22210, $228AC; 7 corpus runs
 */
void Op5_OriginAtScreenCentre(uint16_t *d0, uint32_t *handler, const u16 **script, Vm5Wave *self)
{
    *d0 = (uint16_t)((g_CameraX >> 16) + 0xA0);
    self->origin_x = (int16_t)*d0;
    *d0 = (uint16_t)((g_CameraY >> 16) + 0x70);
    self->origin_y = (int16_t)*d0;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
