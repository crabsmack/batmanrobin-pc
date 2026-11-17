#include "game.h"

/* the turret's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_026164 = 0x026164u;

/* Boss_Stage2_SpawnTurret2 - spawns turret 2 of the stage 2 arena boss (Scr2_026164: bit 2, hull offset ($E7, $63)), an
 * unlinked boss-class object the boss runs inline (Boss_Stage2_MoveHullRunTurrets)
 * In:  x/y = where it starts (Op2_SpawnChildOrWait passes the boss's spawn position)
 * Out: script = Scr2_026164, then Obj_SpawnBossUnlinked: Z set and a6 = the turret, Z clear when
 *      the pool is short
 * Evidence: lea Scr2_026164,a2 / jmp Obj_SpawnBossUnlinked; dc.l at $025C16 (Op2_SpawnChildOrWait field $20); TAS f32140; RAM f32220: x 146. The prototype gains
 *           &a2 (measured a clobber: no caller reads it) so the jump hands the script on
 */
void Boss_Stage2_SpawnTurret2(uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t a4, uint32_t *a6)
{
    *script = Scr2_026164;
    dk::jump(at_Obj_SpawnBossUnlinked);   /* jmp: it returns to our caller */
}
