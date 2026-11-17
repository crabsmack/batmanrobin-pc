#include "game.h"

/* Snd_ResetFade - driver API stub: resets the music fade (Snd_ResetFadeBody with a0 =
 * g_SndFadeLevel: the level 0, and 0 sent to the Z80's fade byte)
 * In:  none
 * Out: d0 as Snd_ResetFadeBody leaves it; g_SndFadeLevel = 0
 * Evidence: move.l a1,-(a7) / lea g_SndFadeLevel,a0 / lea Snd_ResetFadeBody,a1 / jsr (a1) /
 *           movea.l (a7)+,a1 / rts; callers Snd_Init (bra.w, at boot: its rts empties the stack)
 *           and Sound_RestartMusic before every song start; 146 corpus runs
 */
uint32_t Snd_ResetFade(void)
{
    return Snd_ResetFadeBody(&g_SndFadeLevel);
}
