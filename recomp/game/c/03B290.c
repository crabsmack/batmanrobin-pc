#include "game.h"

/* the head's script after the native's jsr $7FE.w (VM2_Enter): Op2_SetBossEnergy, the frame loop */
static constexpr uint32_t Scr2_03B294 = 0x03B294u;

/* Boss_CatHead_DamageSurvived - the cat head took a hit and has energy left: the script goes on
 * (the boss meter shows the new energy, then the head's frame loop)
 * In:  a6 = the head object
 * Out: the script at Scr2_03B294, every register as it came
 * Evidence: jsr ($7FE).w only; bgt.s from Boss_CatHead_TakeDamage ($03B282); TAS f87527-f89334
 *           (179 frames, one fewer than Boss_CatHead_TakeDamage: the killing hit)
 */
void Boss_CatHead_DamageSurvived(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                                 uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5,
                                 uint32_t a6)
{
    dk::jump_script(at_VM2_Enter, Scr2_03B294);
}
