#include "game.h"

/* the a2 script after this native's own jsr $7FE.w: an endless yield until
   Flight_AirshipStartPath points the object's resume at Scr2_02F644 */
static constexpr uint32_t Scr2_02F60A = 0x02F60Au;

/* Flight_AirshipPathObjPark - native script start of the airship path object: leaves its handle
 * in g_FlightAirship path_obj and re-enters the a2 VM on Scr2_02F60A, where it waits until
 * Flight_AirshipStartPath gives it its path
 * In:  self = the new path object (a6); every register the VM re-entry reads
 * Out: g_FlightAirship path_obj = the low word of self; the script runs on (jsr $7FE.w)
 * Evidence: move.w a6,g_SceneVars+$92 / jsr $7FE.w; Op2_Native word $07FC at $2F600 (Tbl_02F600)
 *           runs straight into it; RAM: g_SceneVars+$92 ($F586) = $AE62 at TAS f67740, whose pc is
 *           $2F66A from f67320; TAS f35433
 */
void Flight_AirshipPathObjPark(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                               uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5,
                               uint32_t self)
{
    g_FlightAirship->path_obj = (int16_t)(uint16_t)self;
    dk::jump_script(at_VM2_Enter, Scr2_02F60A);
}
