#include "game.h"

/* the boss script after the native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_01C602 = 0x01C602u;

static constexpr uint16_t BALLOON_SFX_LAND = 0x0B;   /* the landing thud */

/* Boss_JokerBalloon_LandSound - native fragment of the balloon boss script: after a floor touch (dust
 * effect, Shake_StartIfIdle) plays sound $B unless the screen was already shaking, then resumes the
 * script at $1C602
 * In:  the boss object's registers (entered by Op2_Native $7FC at $1C5EC)
 * Out: when it played: d0 low word = $B (then Snd_PlaySfx's d0); the script continues at
 *      Scr2_01C602 (jsr $7FE.w)
 * Evidence: tst.l g_ShakeY / bne / move.w #$B,d0 / jsr Snd_PlaySfx / jsr $7FE.w; the script before
 *           it: BranchNotFloorMask 3, FxSpawnOff $1E/$1BEA, CallNative Shake_StartIfIdle; Snd_PlaySfx
 *           = the sound call of Op2_PlaySound; TAS f19339-f20434 (25 frames). d0 is an output (the
 *           measured contract ends at the jsr $7FE.w; dk::jump_script hands on only outputs)
 */
void Boss_JokerBalloon_LandSound(uint32_t *d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                                 uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5,
                                 uint32_t a6)
{
    if (g_ShakeY == 0) {
        *d0 = (*d0 & 0xFFFF0000u) | BALLOON_SFX_LAND;
        Snd_PlaySfx(d0);
    }
    dk::jump_script(at_VM2_Enter, Scr2_01C602);
}
