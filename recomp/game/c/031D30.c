#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_031D3C = 0x031D3Cu;

/* Boss_DomeCannon_SpawnDomesTurrets - spawns the dome-cannon mid-boss, second form: the core, two
 * left-facing missile domes behind it at (+$68, -$2E)/(+$68, $2F) and two ball turrets in front
 * (Boss_DomeCannon_SpawnBallTurret, fields $30/$26 at (-$68, -$2E)/(-$68, $2F))
 * In:  x/y = the trigger entry's map position
 * Out: script = Scr2_031D3C, then Obj_SpawnEnemy: Z set and a6 = the new parent when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_031D3C,a2 / jmp Obj_SpawnEnemy; trigger entry Tbl_0307C2 +$71A; RAM f72600:
 *           core energy 5120 plus two ball turrets (energy $200); TAS f72564. The prototype gains
 *           &a2 (measured a clobber: Trigger_Fire saves and restores a2 around its callback) so the
 *           jump hands the script on
 */
void Boss_DomeCannon_SpawnDomesTurrets(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *script, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_031D3C;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
