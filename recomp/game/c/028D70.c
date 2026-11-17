#include "game.h"

/* Boss_TurretPod_ClearTurrets - turret pod mini-boss spawner tail: clears its three turret
 * fields before the pod's script spawns them
 * In:  pod = the new pod (a6, after Obj_SpawnBossOrQuit in Boss_TurretPod_Spawn)
 * Out: turret1-3 = 0; Z set (moveq #0,d0: the spawn succeeded; d0 itself is never read)
 * Evidence: clr.w $1E(a6) / clr.w $22(a6) / clr.w $26(a6) / moveq #0,d0 / rts; jmp from $02EA1E,
 *           whose script Scr2_02EA2E then calls Boss_TurretPod_SpawnTurret1-3; TAS f45122 only.
 *           The prototype gains &ccr for the measured ccout(Z)
 */
void Boss_TurretPod_ClearTurrets(TurretPodObj *pod, dk::Flags *ccr)
{
    pod->turret1 = 0;
    pod->turret2 = 0;
    pod->turret3 = 0;
    *ccr = dk::Flags::test(0, 4, ccr->x);   /* moveq #0,d0 */
}
