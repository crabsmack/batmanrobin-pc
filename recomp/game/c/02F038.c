#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02F044 = 0x02F044u;

/* Boss_TurretPod_CreateTurret1 - creates the turret pod's first turret: an unlinked boss-class
 * object (Obj_SpawnBossUnlinked) with Scr2_02F044: energy $800 (1P and 2P), score $C8, attachment
 * offset (0, 6), then the turret body Scr2_02915A
 * In:  x/y = the pod's position (Op2_Native from the pod script: the turret starts there)
 * Out: script = Scr2_02F044, then Obj_SpawnBossUnlinked: Z set and a6 = the turret (an unlinked
 *      boss-class object the pod runs itself), Z clear when the pool is short
 * Evidence: lea Scr2_02F044,a2 / jmp Obj_SpawnBossUnlinked; relative operand of
 *           Boss_TurretPod_SpawnTurret1 at $2EA30; Boss_TurretPod_RunTurret adds the offset from
 *           its a3 +$14/+$16 each frame; TAS f45123. The prototype gains &a2 (measured a clobber:
 *           no caller reads it) so the jump hands the script on
 */
void Boss_TurretPod_CreateTurret1(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_02F044;
    dk::jump(at_Obj_SpawnBossUnlinked);   /* jmp: it returns to our caller */
}
