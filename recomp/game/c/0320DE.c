#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_0320EA = 0x0320EAu;

/* Boss_DomeCannon_SpawnFourDomes - spawns the dome-cannon mid-boss, third form: the core, two
 * left-facing missile domes at (+$71, -$2E)/(+$71, $2F) and two right-facing ones
 * (Boss_DomeCannon_SpawnDomeRight, fields $30/$26 at (-$55, -$2E)/(-$55, $2F))
 * In:  x/y = the trigger entry's map position
 * Out: script = Scr2_0320EA, then Obj_SpawnEnemy: Z set and a6 = the new parent when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_0320EA,a2 / jmp Obj_SpawnEnemy; trigger entry Tbl_0307C2 +$728; TAS f75007.
 *           The prototype gains &a2 (measured a clobber: Trigger_Fire saves and restores a2 around
 *           its callback) so the jump hands the script on
 */
void Boss_DomeCannon_SpawnFourDomes(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *script, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_0320EA;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
