#include "game.h"

/* Boss_TurretPod_RunTurrets - pod native, every frame: clears script var 0, then runs each live
 * turret attached to the pod and ORs the turret's var 0 bit (1/2/4) into it, so var 0 = which
 * turrets are still alive
 * In:  podvm/pod = the pod's VM block and the pod (a4/a6); the rest the pod's registers
 * Out: pod var 0 = the OR of the live turrets' bits; d0-d7 as the last turret left them;
 *      g_VM2SavedSP and a1-a3/a5 preserved (on the stack around the turret runs)
 * Evidence: move.l g_VM2SavedSP,-(a7) / movem.l a1-a3/a5,-(a7) / clr.w $22(a4) / three times
 *           move.w $1E/$22/$26(a6),d0 + jsr Boss_TurretPod_RunTurret / movem.l (a7)+ / move.l
 *           (a7)+,g_VM2SavedSP; op $2CB2 at $028DE0/$028E2C in the pod script Scr2_028D80, which
 *           unlocks its body box (Op2_BranchVar0None) only when var 0 is 0; TAS f45123-f46053. d1-d3/d6/d7
 *           become outputs (measured inputs: the turret scripts change them, verify)
 */
void Boss_TurretPod_RunTurrets(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                               uint32_t *d7, uint32_t a1, uint32_t a2, uint32_t a3, Vm2Block **podvm, uint32_t a5,
                               Vm2Obj **pod)
{
    /* the turrets' VM2_RunObject sets g_VM2SavedSP for their own yields: the pod's is kept on
       the stack, with a1-a3/a5 (movem.l a1-a3/a5,-(a7): a1 lowest) */
    dk::push_long(g_VM2SavedSP);
    dk::push_long(a5);
    dk::push_long(a3);
    dk::push_long(a2);
    dk::push_long(a1);
    const TurretPodObj *turrets = (const TurretPodObj *)*pod;
    (*podvm)->var[0] = 0;
    uint32_t run_a1 = a1;   /* the turret runs leave a1 to the next; the movem restores it */
    for (int slot = 0; slot < 3; slot++) {
        /* each field is read just before its run (a turret's run may change the next) */
        s16 turret = slot == 0 ? turrets->turret1 : slot == 1 ? turrets->turret2 : turrets->turret3;
        *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)turret;
        /* move.w: Z = no turret there, which Boss_TurretPod_RunTurret's beq tests */
        Boss_TurretPod_RunTurret(d0, d1, d2, d3, d4, d5, d6, d7, &run_a1, *podvm, *pod,
                                 dk::Flags::test((uint16_t)turret, 2));
    }
    dk::pop_long();
    dk::pop_long();
    dk::pop_long();
    dk::pop_long();
    g_VM2SavedSP = dk::pop_long();
}
