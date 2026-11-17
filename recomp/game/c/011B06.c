#include "game.h"

/* BossMeter_AddPart - native callback for a multi-part boss: adds this part's energy to the
 * meter's full and current energy.
 * In:  part = the boss part object (a6, +$52 energy)
 * Out: d0 low word = the energy; the flags: add.w d0,(g_BossEnergy).w's
 * Evidence: move.w $52(a6),d0 / add.w d0,(g_BossEnergyMax).w / add.w d0,(g_BossEnergy).w / rts; op
 *           $2CBC pointers in $0261BC and $02918C; TAS frames 32140, 45123
 */
uint16_t BossMeter_AddPart(const Vm2Obj *part, dk::Flags *ccr)
{
    uint16_t energy = (uint16_t)part->energy;
    g_BossEnergyMax = (uint16_t)(g_BossEnergyMax + energy);
    uint16_t before = g_BossEnergy;
    uint16_t sum = (uint16_t)(before + energy);
    g_BossEnergy = sum;
    bool carry = (uint32_t)before + energy > 0xFFFFu;
    bool overflow = ((~(before ^ energy) & (before ^ sum)) & 0x8000u) != 0;
    *ccr = dk::Flags{ carry, (sum & 0x8000u) != 0, sum == 0, overflow, carry };
    return energy;
}

/* BossMeter_Defeated - native callback when a boss dies: zeroes the meter's energy, switches the
 * meter object to its exit script Scr2_011B8E (coroutine slot 0) and ends BOSS TIME
 * (HUD_UnlockJoin).
 * In:  none
 * Out: g_BossEnergy = 0; d0 low word = g_BossMeterObj; no meter: the flags move.w's (Z), else as
 *      HUD_UnlockJoin
 * Evidence: clr.w (g_BossEnergy).w / move.w (g_BossMeterObj).w,d0 / beq rts / movea.w d0,a0 /
 *           movea.w $A(a0),a0 / move.l #$11B8E,$38(a0) / bra.w HUD_UnlockJoin; 11 op $2CBC pointers
 *           in boss scripts (e.g. $0199FA)
 */
uint16_t BossMeter_Defeated(dk::Flags *ccr)
{
    static constexpr uint32_t Scr2_011B8E = 0x011B8Eu;   /* the meter's exit: wait, BossMeter_Free, kill */
    g_BossEnergy = 0;
    uint16_t meter = g_BossMeterObj;
    if (meter == 0) {
        *ccr = dk::Flags::test(meter, 2, ccr->x);
        return meter;
    }
    Vm2Obj *obj = dk::handle<Vm2Obj>((int16_t)meter);
    Vm2Block *vm = dk::handle<Vm2Block>(obj->vm);
    vm->coroutine[0].v = Scr2_011B8E;
    dk::jump(at_HUD_UnlockJoin);
    return meter;
}
