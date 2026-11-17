#include "game.h"

/* the head's script after the native's jsr $7FE.w (VM2_Enter): goto the defeat at $03B2CE */
static constexpr uint32_t Scr2_03B28C = 0x03B28Cu;

enum {
    CATHEAD_STRIKER_PLAYER1 = 1,   /* struck_by: a player's own attack (punch, kick) */
    CATHEAD_STRIKER_PLAYER2 = 2,
};

/* Boss_CatHead_TakeDamage - native fragment of the cat head's hit test: takes the damage of the
 * struck box (and clears it), a quarter of it unless a player's own attack struck (striker id 1
 * or 2, not a shot), from the head's energy; at 0 the script goes on to the defeat, else the
 * survived branch resumes the frame loop
 * In:  head = the head object (a6); g_StruckBox = the box Op2_BranchNotStruck found
 * Out: head->energy lowered (floored at 0); then Scr2_03B28C (energy gone) or
 *      Boss_CatHead_DamageSurvived (energy left)
 * Evidence: movea.l (g_StruckBox).w,a0 / move.w $20(a0),d0 / clr.w $20(a0) / cmpi.w #1/#2,$1A /
 *           lsr.w #2,d0 / sub.w d0,$52(a6) / bgt.s Boss_CatHead_DamageSurvived / clr.w $52(a6) /
 *           jsr ($7FE).w; dropped into after Op2_BranchNotStruck / Tint_FlashC1 at $03B254; TAS
 *           f87527-f89343 (180 frames, the last one the energy hit 0)
 */
void Boss_CatHead_TakeDamage(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                             uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5,
                             Vm2Obj *head)
{
    PTR(HitBox) box;
    box.v = g_StruckBox;
    uint16_t damage = box->damage;
    box->damage = 0;
    uint16_t striker = box->struck_by;
    if (striker != CATHEAD_STRIKER_PLAYER1 && striker != CATHEAD_STRIKER_PLAYER2)
        damage >>= 2;   /* shots do a quarter */
    int16_t left = (int16_t)(head->energy - damage);
    head->energy = left;
    if (left > 0) {
        dk::jump(at_Boss_CatHead_DamageSurvived);
        return;
    }
    head->energy = 0;
    dk::jump_script(at_VM2_Enter, Scr2_03B28C);
}
