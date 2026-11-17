#include "game.h"

/* the turret's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02619C = 0x02619Cu;

/* Boss_Stage2_SpawnTurret4 - spawns turret 4 of the stage 2 arena boss (Scr2_02619C: bit 8, hull offset ($147, $63): the rightmost), an
 * unlinked boss-class object the boss runs inline (Boss_Stage2_MoveHullRunTurrets)
 * In:  x/y = where it starts (Op2_SpawnChildOrWait passes the boss's spawn position)
 * Out: script = Scr2_02619C, then Obj_SpawnBossUnlinked: Z set and a6 = the turret, Z clear when
 *      the pool is short
 * Evidence: lea Scr2_02619C,a2 / jmp Obj_SpawnBossUnlinked; dc.l at $025C26 (field $24); TAS f32140; RAM f32220: x 242. The prototype gains
 *           &a2 (measured a clobber: no caller reads it) so the jump hands the script on
 */
void Boss_Stage2_SpawnTurret4(uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t a4, uint32_t *a6)
{
    *script = Scr2_02619C;
    dk::jump(at_Obj_SpawnBossUnlinked);   /* jmp: it returns to our caller */
}
