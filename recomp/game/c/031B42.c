#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_031B4E = 0x031B4Eu;

/* Boss_DomeCannon_SpawnTwoDomes - spawns the airship's dome-cannon mid-boss, first form: its script
 * spawns the core (Boss_DomeCannon_SpawnCore, child field $2A) and two left-facing missile domes
 * (Boss_DomeCannon_SpawnDomeLeft, $2C/$2E at (-$68, -$2E)/(-$68, $2F)); its coroutine 1 schedules
 * the domes' launch scripts and, after $1A40, the core's boss-meter coroutine
 * In:  x/y = the trigger entry's map position
 * Out: script = Scr2_031B4E, then Obj_SpawnEnemy: Z set and a6 = the new parent when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_031B4E,a2 / jmp Obj_SpawnEnemy; trigger entry Tbl_0307C2 +$70C; the script's
 *           Op2_SpawnChildOrWait / Op2_SyncChild / Op2_StartChildCoroutine1Rel, then body $3050A runs
 *           the children (Op2_RunChild); TAS f70036 (core destroyed by f71100). The prototype gains
 *           &a2 (measured a clobber: Trigger_Fire saves and restores a2 around its callback) so the
 *           jump hands the script on
 */
void Boss_DomeCannon_SpawnTwoDomes(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *script, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_031B4E;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
