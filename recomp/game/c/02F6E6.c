#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02F6F2 = 0x02F6F2u;

/* Flight_SpawnRedNavLightFlip - spawns the red navigation light H-flipped (attr xor $08, anim
 * $2F7A0, body $2F746)
 * In:  x/y = the trigger entry's map position (Flight_AirshipRunTriggers then moves it to world space)
 * Out: script = Scr2_02F6F2, then Obj_SpawnEnemy: Z set and a6 = the new object when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_02F6F2,a2 / jmp Obj_SpawnEnemy; two trigger entries in Tbl_0307C2 at map
 *           ($ECC, $CB/$115), the far end of the hull; TAS f75442. The prototype gains &a2
 *           (measured a clobber: no caller reads it) so the jump hands the script on
 */
void Flight_SpawnRedNavLightFlip(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *script, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_02F6F2;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
