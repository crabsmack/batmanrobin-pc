#include "game.h"

/* the death branch's script after the native's jsr $7FE.w (VM2_Enter): Op2_Kill */
static constexpr uint32_t Scr2_041ED0 = 0x041ED0u;

/* Enemy_GasOrb_DeathReleaseFrame - native fragment at the end of the gas orb's death branch:
 * releases the jet-puff sprite frame it holds, then the script goes on to Op2_Kill
 * In:  orb = the gas orb (a6: the puff frame +$2A)
 * Out: Spr_ReleaseFrame's, then the script at Scr2_041ED0
 * Evidence: move.w $2A(a6),d2 / jsr Spr_ReleaseFrame / jsr ($7FE).w; word $07FC at $41EC0 after
 *           Op2_AwardScoreToHitter and the explosion in Scr2_041EA4; TAS 18 frames 124216-130141
 */
void Enemy_GasOrb_DeathReleaseFrame(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                                    uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5,
                                    const GasOrbObj *orb)
{
    uint16_t w0 = (uint16_t)d0, frame = orb->puff_frame;
    uint32_t a0 = 0;
    Spr_ReleaseFrame(&w0, &frame, &a0);
    dk::jump_script(at_VM2_Enter, Scr2_041ED0);
}
