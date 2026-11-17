#include "game.h"

/* the airship hull's trigger table (130 entries in map coordinates: lights, turrets, domes, pipe
   cannons, the dome-cannon boss) and the path object's script (speed $100, music 5, cruise x $AA,
   a Y oscillator, a loop calling Flight_AirshipSetMapOrigin); ROM, batmanrobin.asm */
static constexpr uint32_t Tbl_0307C2 = 0x0307C2u;
static constexpr uint32_t Scr2_02F644 = 0x02F644u;

/* where the path object, and so the hull map's offset, starts */
static constexpr int16_t FLIGHT_AIRSHIP_START_X = -0x154;
static constexpr int16_t FLIGHT_AIRSHIP_START_Y = 0x2C;

/* Flight_AirshipStartPath - starts the airship hull part of the flight: loads the hull's trigger
 * table, points the parked path object's script at its path and puts it, and the map offset it
 * publishes, at (-$154, $2C)
 * In:  g_FlightAirship path_obj (Flight_AirshipPathObjPark)
 * Out: Trigger_LoadX's results (d6/d7, a0-a3: g_TriggerList filled from Tbl_0307C2); vm/self = the
 *      path object's VM block and the object (a4/a6, sign-extended handles), its resume =
 *      Scr2_02F644 and x/y = map_x/map_y = (-$154, $2C); d1 = $2C (d0 = -$154 is a clobber)
 * Evidence: lea Tbl_0307C2,a0 / jsr Trigger_LoadX / movea.w g_SceneVars+$92,a6 / movea.w $A(a6),a4 /
 *           move.l #$2F644,$2A(a4) / move.w #$FEAC,d0 / move.w #$2C,d1 / d0 -> $94 and $3E(a6), d1 ->
 *           $96 and $42(a6); jsr from Flight_StartAirship ($33398); TAS f67262
 */
void Flight_AirshipStartPath(uint16_t *d1, uint32_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2,
                             uint32_t *a3, uint32_t *vm, uint32_t *self)
{
    /* Trigger_LoadX on the ROM table; the table is never empty, so its src / dst (a2 / a3) are
       written before they are read */
    PTR(const u8) rom;
    rom.v = Tbl_0307C2;
    const u8 *t = rom.get();
    TriggerKey *end = nullptr, *src = nullptr, *dst = nullptr;
    Trigger_LoadX(d6, d7, &t, &end, &src, &dst);
    *a0 = dk::addr(t);
    *a1 = dk::addr(end);
    *a2 = dk::addr(src);
    *a3 = dk::addr(dst);

    int16_t handle = g_FlightAirship->path_obj;
    *self = (uint32_t)(int32_t)handle;                         /* movea.w */
    Vm2Obj *path = dk::handle<Vm2Obj>(handle);
    *vm = (uint32_t)(int32_t)path->vm;
    Vm2Block *block = dk::handle<Vm2Block>(path->vm);
    block->resume.v = Scr2_02F644;
    *d1 = (uint16_t)FLIGHT_AIRSHIP_START_Y;
    g_FlightAirship->map_x = FLIGHT_AIRSHIP_START_X;
    path->x = FLIGHT_AIRSHIP_START_X;
    g_FlightAirship->map_y = FLIGHT_AIRSHIP_START_Y;
    path->y = FLIGHT_AIRSHIP_START_Y;
}
