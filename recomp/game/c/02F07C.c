#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02F088 = 0x02F088u;

/* Boss_TurretPod_CreateTurret3 - spawns the turret pod's third turret as an unlinked boss-class
 * object: Scr2_02F088 sets energy $800 (1P/2P), score $C8, the attachment offset (0, $2B) and
 * enters the shared turret body at $29174
 * In:  x/y = the pod's position (Op2_Native from the pod script: the turret starts there)
 * Out: script = Scr2_02F088, then Obj_SpawnBossUnlinked: Z set and a6 = the turret (an unlinked
 *      boss-class object the pod runs itself), Z clear when the pool is short
 * Evidence: lea Scr2_02F088,a2 / jmp Obj_SpawnBossUnlinked; relative word at $2EA3E in the pod
 *           script Scr2_02EA2E (op $2CB2 -> Boss_TurretPod_SpawnTurret3, var 0 bit 4); its siblings
 *           $2F038/$2F05A use offsets (0, 6)/(0, -$11); TAS f45123 only (the pod fight
 *           45122-46053). The prototype gains &a2 (measured a clobber: no caller reads it) so the
 *           jump hands the script on
 */
void Boss_TurretPod_CreateTurret3(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_02F088;
    dk::jump(at_Obj_SpawnBossUnlinked);   /* jmp: it returns to our caller */
}
