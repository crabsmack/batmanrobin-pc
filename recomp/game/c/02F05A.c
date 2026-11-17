#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02F066 = 0x02F066u;

/* Boss_TurretPod_CreateTurret2 - creates the turret pod's second turret (Scr2_02F066): energy $800,
 * score $C8, attachment offset (0, -$11), then the turret body at $29174
 * In:  x/y = the pod's position (Op2_Native from the pod script: the turret starts there)
 * Out: script = Scr2_02F066, then Obj_SpawnBossUnlinked: Z set and a6 = the turret (an unlinked
 *      boss-class object the pod runs itself), Z clear when the pool is short
 * Evidence: lea Scr2_02F066,a2 / jmp Obj_SpawnBossUnlinked; relative operand of
 *           Boss_TurretPod_SpawnTurret2 at $2EA36; the third turret ($2F07C, offset (0, $2B))
 *           follows; TAS f45123. The prototype gains &a2 (measured a clobber: no caller reads it)
 *           so the jump hands the script on
 */
void Boss_TurretPod_CreateTurret2(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_02F066;
    dk::jump(at_Obj_SpawnBossUnlinked);   /* jmp: it returns to our caller */
}
