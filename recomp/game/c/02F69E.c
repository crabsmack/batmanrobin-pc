#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02F6AA = 0x02F6AAu;

/* Flight_SpawnStrobeLight - spawns a white strobe light on the airship hull: attr palette 0, anim
 * $2F764 (four flashes of frames $42D6/$42E0, then dark 15-45 frames: Op2_SetAnimDelayRand $F00,
 * $1E00), body $2F746 (map-anchored, drawn behind, no box)
 * In:  x/y = the trigger entry's map position (Flight_AirshipRunTriggers then moves it to world space)
 * Out: script = Scr2_02F6AA, then Obj_SpawnEnemy: Z set and a6 = the new object when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_02F6AA,a2 / jmp Obj_SpawnEnemy; 32 trigger entries in Tbl_0307C2 (pairs at map
 *           y $CB/$115 and $77/$16A); frames rendered: a white half-disc glow; TAS 67420-75171 (20
 *           spawns). Raw, never run: an H-flipped twin at $2F6B6 (Scr2_02F6C2). The prototype gains
 *           &a2 (measured a clobber: no caller reads it) so the jump hands the script on
 */
void Flight_SpawnStrobeLight(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *script, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_02F6AA;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
