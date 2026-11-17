#include "game.h"

/* the object's a2 script: one Op2_Native word ($07FC) whose native is Flight_AirshipPathObjPark,
   right after this spawner's code (batmanrobin.asm) */
static constexpr uint32_t Tbl_02F600 = 0x02F600u;

/* Flight_SpawnAirshipPathObj - spawns the flying section's airship path object (enemy class): its
 * script is the native op at Tbl_02F600, which runs Flight_AirshipPathObjPark
 * In:  x/y = whatever the scene init left in d4/d5 (the path object places itself later:
 *      Flight_AirshipStartPath)
 * Out: script = Tbl_02F600, then Obj_SpawnEnemy's d0/d1 and blocks (a3-a5); a6 and d7 are its
 *      clobbers here (the scene init reads neither)
 * Evidence: lea Tbl_02F600,a2 / jmp Obj_SpawnEnemy; jsr from $33274 before the scene's VM6_Run; TAS
 *           f35420, level-skip scenario 4 times (each flight-section start)
 */
void Flight_SpawnAirshipPathObj(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t a6)
{
    *script = Tbl_02F600;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
