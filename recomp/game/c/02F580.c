#include "game.h"

/* Flight_AirshipSetMapOrigin - native op of the airship path object: publishes its position as the
 * hull map's scroll offset (the map position of world 0,0: the airship map moves with the path
 * object), then runs the hull's triggers
 * In:  self = the path object (a6, Scr2_02F644)
 * Out: g_FlightAirship map_x/map_y = its x/y; then Flight_AirshipRunTriggers (falls into it): every
 *      register as that hands them back
 * Evidence: move.w $3E(a6),g_SceneVars+$94 / move.w $42(a6),g_SceneVars+$96, falling into
 *           Flight_AirshipRunTriggers; op $2CB2 at $2F674 in the path loop (step velocity,
 *           oscillator, ApplyMove, this, yield); Flight_FrameAirship ($333B2) sets plane A to the
 *           offset + g_ViewX/Y; TAS 67264-77147 every frame
 */
void Flight_AirshipSetMapOrigin(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t *d4, uint32_t *d5,
                                uint32_t *d6, uint32_t *d7, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4,
                                uint32_t a5, uint32_t self)
{
    PTR(const Vm2Obj) path;
    path.v = self;
    g_FlightAirship->map_x = path->x;
    g_FlightAirship->map_y = path->y;
    dk::jump(at_Flight_AirshipRunTriggers);   /* the fall-through into the next file */
}
