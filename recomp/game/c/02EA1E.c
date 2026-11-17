#include "game.h"

/* the pod's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02EA2E = 0x02EA2Eu;

/* Boss_TurretPod_Spawn - wave spawner of the turret pod mini-boss: spawns Scr2_02EA2E as a
 * boss-class object and clears its three turret fields; the pod's script then creates the turrets
 * (Boss_TurretPod_CreateTurret1-3)
 * In:  x/y = where the wave entry places it
 * Out: script = Scr2_02EA2E, then Obj_SpawnBossOrQuit (a3/a4/a6 = the pod's blocks, d1/d7) and
 *      Boss_TurretPod_ClearTurrets: turret handles $1E/$22/$26 = 0, Z set (moveq #0,d0; d0 itself
 *      is never read); a short pool returns past this to the wave loop
 * Evidence: lea Scr2_02EA2E,a2 / jsr Obj_SpawnBossOrQuit / jmp Boss_TurretPod_ClearTurrets;
 *           Scr5_021D3A +$2C2 (x $14A, screen y $70, drop action $2E); the script's first ops are
 *           the Boss_TurretPod_SpawnTurret1-3 natives (op $2CB2) on $2F038/$2F05A/$2F07C; TAS f45122
 */
void Boss_TurretPod_Spawn(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_02EA2E;
    Obj_SpawnBossOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Boss_TurretPod_ClearTurrets);   /* jmp: its a6 = the new pod */
}
