#include "game.h"

/* the two objects' a2 scripts (ROM data; batmanrobin.asm) */
static constexpr uint32_t Scr2_029598 = 0x029598u;   /* the radius object: eases its x to the beam radius */
static constexpr uint32_t Scr2_02DF94 = 0x02DF94u;   /* the ship */

static constexpr uint16_t BEAMSHIP_BLOCKS = 4;   /* a scripted object: itself, motion, VM, sprite */

/* Boss_BeamShip_Spawn - spawns the stage 2 flight section's final boss: an unlinked radius object
 * (Scr2_029598) and the ship itself (Scr2_02DF94), each holding the other (ship +$22 = the radius
 * object, radius object +$1E = the ship)
 * In:  d4/d5 = x/y (the scene sets $172, $70)
 * Out: a3-a6 = the ship's blocks, d1 as Obj_Spawn left it; when the pool is short: the radius
 *      object freed again (a6 = it) or nothing spawned (d0 = -1: a clobber, no caller reads it)
 * Evidence: jsr Obj_SpawnScriptedUnlinked / bne / move.l #$29598,$2A(a4) / movea.l a6,a1 / jsr
 *           Obj_SpawnScripted / bne / move.l #$2DF94,$2A(a4) / move.l a1,$22(a6) / move.l a6,$1E(a1)
 *           / moveq #0,d0, or movea.l a1,a6 / jsr Obj_FreeWithBlocks / moveq #-1,d0; called by the
 *           scene init $33A62; TAS f77184; f078000 shows the white ship and its beam. The C runs
 *           Obj_SpawnScriptedUnlinked's two instructions (lea VM2_RunObject,a0 / moveq #4,d7) and
 *           calls Obj_SpawnUnlinked, whose d0 = -1 / 0 is the Z the bne reads (its own contract
 *           hands back only Z)
 */
void Boss_BeamShip_Spawn(uint32_t *d1, uint16_t d4, uint16_t d5, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, a0 = at_VM2_RunObject;
    uint16_t blocks = BEAMSHIP_BLOCKS, w1 = (uint16_t)*d1;
    Obj_SpawnUnlinked(&d0, &w1, d4, d5, &blocks, &a0, a3, a4, a5, a6);
    *d1 = (*d1 & 0xFFFF0000u) | w1;
    if (d0 != 0)                        /* bne: Z clear exactly when the pool was short (d0 = -1) */
        return;
    PTR(Vm2Block) vm;
    vm.v = *a4;
    vm->resume.v = Scr2_029598;
    uint32_t radius = *a6;              /* movea.l a6,a1 (Obj_Spawn saves a1 round its work) */
    uint32_t d7 = 0;
    w1 = (uint16_t)*d1;
    Obj_SpawnScripted(&d0, &w1, d4, d5, &d7, &a0, a3, a4, a5, a6);
    *d1 = (*d1 & 0xFFFF0000u) | w1;
    if (d0 != 0) {
        *a6 = radius;                   /* movea.l a1,a6: free the radius object again */
        uint32_t r1 = *d1, r_a1 = radius;
        dk::Flags free_ccr{};           /* its flags in: Obj_SpawnScripted's (Z clear); none read after */
        Obj_FreeWithBlocks(&d0, &r1, &a0, &r_a1, *a6, &free_ccr);
        *d1 = r1;
        return;
    }
    vm.v = *a4;
    vm->resume.v = Scr2_02DF94;
    PTR(BeamShipObj) ship;
    ship.v = *a6;
    ship->radius.v = radius;
    PTR(BeamRadiusObj) disc;
    disc.v = radius;
    disc->ship.v = *a6;
}
