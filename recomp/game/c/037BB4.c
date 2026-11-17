#include "game.h"

/* the chain script after the native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_037BC4 = 0x037BC4u;

/* ToyChain_PushByHit - native fragment of the drop-in chain body ($037B8A): when
 * ToyChain_TakeLinkHits reports a hit, the struck link's knockback is added to the chain's sway
 * speed (a hit swings the chain away from the attacker), then the script resumes
 * In:  g_StruckBox = the struck link's box; mot = the chain's motion block; the rest the
 *      object's registers (for the script)
 * Out: mot->vel_x += the knockback; d0 = the knockback, a0 = the box; the script continues at
 *      Scr2_037BC4 (jsr $7FE.w)
 * Evidence: movea.l g_StruckBox,a0 / move.l $22(a0),d0 / add.l d0,$18(a3) / jsr $7FE; reached
 *           through the a2 op $07FC after the Op2_CallNativeRelSwitch at $037BA8; TAS
 *           f92284-f93126 (39 frames). d0 and a0 are outputs (the measured contract ends at the
 *           jsr $7FE.w; dk::jump_script hands on only outputs)
 */
void ToyChain_PushByHit(uint32_t *d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                        uint32_t d6, uint32_t d7, uint32_t *a0, uint32_t a1, Vm2Motion *mot, uint32_t a4,
                        uint32_t a5, uint32_t a6)
{
    PTR(const HitBox) box;
    box.v = g_StruckBox;
    *a0 = g_StruckBox;
    *d0 = (uint32_t)(int32_t)box->knockback;
    mot->vel_x = (int32_t)((uint32_t)mot->vel_x + *d0);
    dk::jump_script(at_VM2_Enter, Scr2_037BC4);
}
