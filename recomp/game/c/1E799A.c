#include "game.h"

/* Snd_ResetFadeBody - driver body of Snd_ResetFade: clears the fade level and sends 0 to the Z80's
 * fade byte
 * In:  level (a0) = g_SndFadeLevel
 * Out: *level = 0; Z80 RAM $0E67 = 0; d0 = 0
 * Evidence: clr.w (a0) / moveq #0,d0 / bsr.w Snd_WriteFadeByte / rts; 146 corpus runs
 */
uint32_t Snd_ResetFadeBody(u16 *level)
{
    *level = 0;
    Snd_WriteFadeByte(0);
    return 0;
}
