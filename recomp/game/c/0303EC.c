#include "game.h"

/* Map-anchored hull objects keep their airship map position in the drawn-position fields
 * (draw_x/draw_y, +$4E/+$50); Flight_AirshipMapToWorld turns it into the world x/y each frame. */

/* Enemy_PipeCannon_PipePhase - animation switch native of the pipe cannon: bit 1 of its map y picks
 * one of two frames whose pipe stripes are offset, so the pipe seems to slide past while the
 * cannon moves
 * In:  self = the pipe cannon (a6; map y in draw_y)
 * Out: the case for Op2_CallNativeRelSwitch (d0 = 0 or 1; its high word unchanged)
 * Evidence: move.w $50(a6),d0 / lsr.w #1,d0 / andi.w #1,d0 / rts; op $2D30 at $304BE (intact frames
 *           $415A/$4164) and $304F4 (broken frames $416E/$4178) in the anim scripts of Scr2_030416;
 *           TAS 69665-74542 (348 frames)
 */
uint16_t Enemy_PipeCannon_PipePhase(uint32_t self)
{
    PTR(const Vm2Obj) obj;
    obj.v = self;
    return (uint16_t)(((uint16_t)obj->draw_y >> 1) & 1);
}

/* Flight_AirshipLoadMapPos - loads a map-anchored object's map position into its x/y, so the move
 * ops that follow integrate its motion in airship map space
 * In:  self = the object (a6)
 * Out: self->x/y (integer words; the fractions stay) = draw_x/draw_y
 * Evidence: move.w $4E(a6),$3E(a6) / move.w $50(a6),$42(a6) / rts; op $2CB2 at $30432 and $30472 in
 *           the pipe cannon's loop (then Op2_StepVelocity, Op2_ApplyMove, Flight_AirshipStoreMapPos);
 *           TAS 69585-74542
 */
void Flight_AirshipLoadMapPos(uint32_t self)
{
    PTR(Vm2Obj) obj;
    obj.v = self;
    obj->x = obj->draw_x;
    obj->y = obj->draw_y;
}

/* Flight_AirshipStoreMapPos - stores a moved object's x/y as its map position, then converts back
 * to the world x/y (Flight_AirshipMapToWorld)
 * In:  self = the object (a6), its new map-space position in x/y
 * Out: draw_x/draw_y = x/y; then Flight_AirshipMapToWorld: x/y (and d4/d5) = the world position
 * Evidence: move.w $3E(a6),$4E(a6) / move.w $42(a6),$50(a6) / bra.w Flight_AirshipMapToWorld; op
 *           $2CB2 at $3047C in the pipe cannon's loop, right after Op2_ApplyMove; TAS 69585-74542
 */
void Flight_AirshipStoreMapPos(uint16_t *x, uint16_t *y, uint32_t self)
{
    PTR(Vm2Obj) obj;
    obj.v = self;
    obj->draw_x = obj->x;
    obj->draw_y = obj->y;
    dk::jump(at_Flight_AirshipMapToWorld);   /* bra: it returns to our caller */
}
