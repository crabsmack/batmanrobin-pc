#include "game.h"

/* Boss_BeamShip_RunRadiusObj - runs the ship's unlinked radius object inline, its target point x
 * set to the ship's var 0 (the wanted beam radius) so the object's x eases toward it
 * In:  shipvm/ship = the ship's VM block and the ship (a4/a6; falls in from
 *      Boss_BeamShip_SetShipVertex); the rest its registers
 * Out: the radius object ran one frame of its script; a1-a6 and g_VM2SavedSP preserved (on the
 *      stack round the run); d0-d7 as the radius object's script left them (outputs: measured
 *      inputs, but verify saw the script change d1)
 * Evidence: movem.l a1-a6,-(a7) / move.l g_VM2SavedSP,-(a7) / movea.l $22(a6),a6 / movea.w $8(a6),a0
 *           / move.w $22(a4),$14(a0) / jsr VM2_RunObject / move.l (a7)+,g_VM2SavedSP / movem.l
 *           (a7)+,a1-a6; the radius object's script Scr2_029598 seeks its screen x to the target
 *           (Op2_SeekScreenX) and Boss_BeamShip_RadiusFromX copies that x to g_BeamRadius; TAS
 *           f77185-f79028 (1503 frames)
 */
void Boss_BeamShip_RunRadiusObj(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                                uint32_t *d7, uint32_t a1, uint32_t a2, uint32_t a3, const Vm2Block *shipvm, uint32_t a5,
                                const BeamShipObj *ship)
{
    /* movem.l a1-a6,-(a7): a1 lowest; then the ship's saved stack pointer (the radius object's
       VM2_RunObject sets its own) */
    dk::push_long(dk::addr(ship));
    dk::push_long(dk::addr(shipvm));
    dk::push_long(a5);
    dk::push_long(a3);
    dk::push_long(a2);
    dk::push_long(a1);
    dk::push_long(g_VM2SavedSP);
    Vm2Obj *radius = ship->radius;
    dk::handle<Vm2Motion>(radius->motion)->point_x = (s16)shipvm->var[0];
    /* VM2_RunObject loads a2-a5 from the radius object itself: the values going in are not read */
    uint32_t handler = 0, r_a1 = a1;
    const u16 *script = 0;
    void *motion = 0, *sprite = 0;
    Vm2Block *vm = 0;
    VM2_RunObject(d0, d1, d2, d3, d4, d5, d6, d7, &handler, &r_a1, &script, &motion, &vm, &sprite, radius);
    g_VM2SavedSP = dk::pop_long();
    for (int i = 0; i < 6; i++)
        dk::pop_long();
}
