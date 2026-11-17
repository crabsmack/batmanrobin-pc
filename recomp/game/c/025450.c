#include "game.h"

/* the barrel's script after this fragment's jsr VM2_Enter (ROM; batmanrobin.asm) */
static constexpr uint32_t Scr2_025466 = 0x025466u;

/* Enemy_Barrel_KnockByHit - a2 native fragment of the barrel's script: when a struck barrel is hit by
 * the attacker kind of its switch, adds half the hit's knockback (box +$22) to its x velocity and x
 * target speed, then resumes the script
 * In:  the barrel's registers (Op2_Native at $02544E): a3 = its motion block; g_StruckBox
 * Out: velocity x / target speed x += knockback / 2; the script continues at Scr2_025466 (jsr
 *      VM2_Enter) with d0 = that half and a0 = the box, every other register as it came
 * Evidence: movea.l g_StruckBox,a0 / move.l $22(a0),d0 / asr.l #1,d0 / add.l d0,$18(a3) / add.l
 *           d0,$24(a3) / jsr VM2_Enter; Op2_Native at $02544E in Scr2_0253DC (Enemy_Barrel_Spawn's
 *           barrel), after Op2_SwitchAttacker; no corpus run reaches it (crane-crash 2026-10-30);
 *           025450.syn checks it. Prototype: d0 and a0 added as outputs (the script resumes with them)
 */
void Enemy_Barrel_KnockByHit(uint32_t *d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                             uint32_t d6, uint32_t d7, uint32_t *a0, uint32_t a1, uint32_t a3, uint32_t a4,
                             uint32_t a5, uint32_t a6)
{
    PTR(const HitBox) box;
    box.v = g_StruckBox;
    *a0 = box.v;
    int32_t half = box->knockback >> 1;                    /* asr.l #1 */
    *d0 = (uint32_t)half;
    PTR(Vm2Motion) mot;
    mot.v = a3;
    mot->vel_x += half;
    mot->target_x += half;
    dk::jump_script(at_VM2_Enter, Scr2_025466);
}
