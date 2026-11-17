#include "game.h"

/* Boss_TurretPod_RunTurret - runs one turret of the pod inline: places it at the pod's x/y plus
 * its attachment offset (the turret's motion point), runs its script (VM2_RunObject) and ORs its
 * var 0 bit into the pod's
 * In:  d0.w = the turret's object handle; ccr = tst.w of it (Z = none: returns at once; the beq
 *      reads Z, not d0: Boss_TurretPod_RunTurrets' move.w leaves it; verify, TAS: with Z left
 *      stale the generated beq skipped turrets, 3 display nodes fewer), podvm/pod = the pod's VM
 *      block and the pod (a4/a6)
 * Out: d0-d7/a1 as the turret's script left them, then d0.w = its var 0; pod var 0 |= it;
 *      a4/a6 kept (on the stack around the run). d1-d3/d6/d7/a1 outputs (measured inputs: verify
 *      saw the turret scripts change d1)
 * Evidence: beq / movem.l a4/a6,-(a7) / move.w $3E/$42(a6),d4/d5 / movea.w d0,a6 / movea.w $8(a6),a3
 *           / add.w $14/$16(a3) / move.w d4/d5,$3E/$42(a6) / jsr VM2_RunObject / move.w $22(a4),d0 /
 *           movem.l (a7)+,a4/a6 / or.w d0,$22(a4); called 3 times by Boss_TurretPod_RunTurrets; the
 *           turret bodies (Scr2_02915A) set the offset with Op2_SetTarget (0, 6), (0, -$11) ...
 */
void Boss_TurretPod_RunTurret(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                              uint32_t *d7, uint32_t *a1, Vm2Block *podvm, Vm2Obj *pod, dk::Flags ccr)
{
    if (ccr.z)
        return;
    dk::push_long(dk::addr(pod));     /* movem.l a4/a6,-(a7): a4 lowest */
    dk::push_long(dk::addr(podvm));
    Vm2Obj *turret = dk::handle<Vm2Obj>((int16_t)(uint16_t)*d0);
    const Vm2Motion *offset = dk::handle<const Vm2Motion>(turret->motion);
    uint16_t x = (uint16_t)(pod->x + offset->point_x);
    uint16_t y = (uint16_t)(pod->y + offset->point_y);
    *d4 = (*d4 & 0xFFFF0000u) | x;
    *d5 = (*d5 & 0xFFFF0000u) | y;
    turret->x = (s16)x;
    turret->y = (s16)y;
    /* VM2_RunObject loads a2-a5 from the turret itself: the values going in are not read */
    uint32_t handler = 0;
    const u16 *script = 0;
    void *motion = 0, *sprite = 0;
    Vm2Block *vm = 0;
    VM2_RunObject(d0, d1, d2, d3, d4, d5, d6, d7, &handler, a1, &script, &motion, &vm, &sprite, turret);
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)vm->var[0];   /* a4 = the turret's block after its run */
    dk::pop_long();
    dk::pop_long();
    podvm->var[0] = (u16)(podvm->var[0] | (uint16_t)*d0);
}

/* the shared body of Boss_TurretPod_SpawnTurret1-3: calls the spawner the script's relative word
 * names with a1-a6 saved on the stack, and on success gives the new turret its var 0 bit and
 * keeps its handle in the pod's field */
static void spawn_turret(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a1,
                         const s16 **script, uint32_t a3, uint32_t a4, uint32_t a5, TurretPodObj *pod, uint16_t bit,
                         s16 TurretPodObj::*field)
{
    const s16 *at = *script;
    uint32_t spawner = dk::addr(at) + (uint32_t)(int32_t)*at;   /* movea.l a2,a0 / adda.w (a2)+,a0 */
    (*script)++;
    /* movem.l a1-a6,-(a7): a1 lowest */
    dk::push_long(dk::addr(pod));
    dk::push_long(a5);
    dk::push_long(a4);
    dk::push_long(a3);
    dk::push_long(dk::addr(*script));
    dk::push_long(a1);
    uint32_t new_vm = a4, turret = dk::addr(pod);
    bool spawned = TurretSpawnerFn(spawner, d0, d1, d4, d5, d7, &new_vm, &turret);
    if (spawned) {
        PTR(Vm2Block) vm;
        vm.v = new_vm;                /* a4 = the new turret's VM block until the movem */
        vm->var[0] = bit;
    }
    for (int i = 0; i < 6; i++)       /* movem.l (a7)+,a1-a6: the pod's registers back */
        dk::pop_long();
    if (spawned)
        pod->*field = (s16)(uint16_t)turret;   /* move.w a0: the new turret's handle */
}

/* Boss_TurretPod_SpawnTurret1 - pod native: calls the spawner at the script's relative word and,
 * on success, gives the new turret var 0 = 1 and keeps it in the pod's turret1
 * In:  script = a word offset (from itself) to an unlinked-object spawner; pod = the pod (a6);
 *      a1/a3-a5 the pod's (saved round the call)
 * Out: script past the word; d0/d1/d7 as the spawner left them; turret1 = the turret's handle
 * Evidence: movea.l a2,a0 / adda.w (a2)+,a0 / movem.l a1-a6,-(a7) / jsr (a0) / bne / move.w
 *           #1,$22(a4) / movea.l a6,a0 / movem.l (a7)+,a1-a6 / move.w a0,$1E(a6); op $2CB2 at
 *           $02EA2E (spawner Boss_TurretPod_CreateTurret1 $02F038); TAS f45122. The prototype gains
 *           a1/a3/a5 (inputs: the movem saves them on the stack)
 */
void Boss_TurretPod_SpawnTurret1(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a1,
                                 const s16 **script, uint32_t a3, uint32_t a4, uint32_t a5, TurretPodObj *pod)
{
    spawn_turret(d0, d1, d4, d5, d7, a1, script, a3, a4, a5, pod, 1, &TurretPodObj::turret1);
}

/* Boss_TurretPod_SpawnTurret2 - as Boss_TurretPod_SpawnTurret1 for the second turret: var 0 = 2,
 * kept in turret2
 * In:  script = a word offset to a spawner; pod = the pod (a6)
 * Out: script past the word; turret2 = the turret's handle
 * Evidence: the same code with move.w #2,$22(a4) / move.w a0,$22(a6); op $2CB2 at $02EA34
 *           (spawner $02F05A); fuzz-45000 runs
 */
void Boss_TurretPod_SpawnTurret2(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a1,
                                 const s16 **script, uint32_t a3, uint32_t a4, uint32_t a5, TurretPodObj *pod)
{
    spawn_turret(d0, d1, d4, d5, d7, a1, script, a3, a4, a5, pod, 2, &TurretPodObj::turret2);
}

/* Boss_TurretPod_SpawnTurret3 - as Boss_TurretPod_SpawnTurret1 for the third turret: var 0 = 4,
 * kept in turret3
 * In:  script = a word offset to a spawner; pod = the pod (a6)
 * Out: script past the word; turret3 = the turret's handle
 * Evidence: the same code with move.w #4,$22(a4) / move.w a0,$26(a6); op $2CB2 at $02EA3A
 *           (spawner $02F07C); TAS f45122
 */
void Boss_TurretPod_SpawnTurret3(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a1,
                                 const s16 **script, uint32_t a3, uint32_t a4, uint32_t a5, TurretPodObj *pod)
{
    spawn_turret(d0, d1, d4, d5, d7, a1, script, a3, a4, a5, pod, 4, &TurretPodObj::turret3);
}
