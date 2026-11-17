#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02F70A = 0x02F70Au;

/* Flight_SpawnGreenNavLight - spawns a green navigation light on the airship hull: anim $2F7BC
 * cycles frames $42FE/$4308 (green glow), body $2F746
 * In:  x/y = the trigger entry's map position (Flight_AirshipRunTriggers then moves it to world space)
 * Out: script = Scr2_02F70A, then Obj_SpawnEnemy: Z set and a6 = the new object when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_02F70A,a2 / jmp Obj_SpawnEnemy; trigger entry Tbl_0307C2+$2A at map (26,
 *           $168); frames rendered; TAS f67264. Raw, never run: an H-flipped twin at $2F716
 *           (Scr2_02F722). The prototype gains &a2 (measured a clobber: no caller reads it) so the
 *           jump hands the script on
 */
void Flight_SpawnGreenNavLight(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *script, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_02F70A;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
