#include "game.h"

/* Flight_AirshipMapToWorld - places a map-anchored object: its world position = its map position
 * (kept in the drawn-position fields) minus the hull map's scroll offset, so it stays fixed on the
 * scrolling airship map
 * In:  self = the object (a6), its map position in draw_x/draw_y ($4E/$50); g_FlightAirship
 *      map_x/map_y
 * Out: self->x/y (whole pixels; the fractions stay) and x/y (d4/d5 low words) = the world position
 * Evidence: move.w $4E(a6),d4 / move.w $50(a6),d5 / sub.w g_SceneVars+$94,d4 / sub.w
 *           g_SceneVars+$96,d5 / move.w d4,$3E(a6) / move.w d5,$42(a6); op $2CB2 in every body of
 *           the section (lights $2F74A, hull turret $2F9B0, domes $2FC86, ...) and from
 *           Flight_AirshipRunTriggers; TAS 67264-76867 (9566 frames)
 */
void Flight_AirshipMapToWorld(uint16_t *x, uint16_t *y, uint32_t self)
{
    PTR(Vm2Obj) obj;
    obj.v = self;
    *x = (uint16_t)(obj->draw_x - g_FlightAirship->map_x);
    *y = (uint16_t)(obj->draw_y - g_FlightAirship->map_y);
    obj->x = (int16_t)*x;
    obj->y = (int16_t)*y;
}
