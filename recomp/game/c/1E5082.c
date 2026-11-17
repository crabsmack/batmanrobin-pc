#include "game.h"

/* Snd_StopChannels - driver API stub: switches the six music channels off
 * (Snd_StopChannelsBody)
 * In:  none
 * Out: the Z80 channel records' on flags = 0; d7 = $FFFF (the body's dbra)
 * Evidence: move.l a1,-(a7) / lea Snd_StopChannelsBody,a1 / jsr (a1) / movea.l (a7)+,a1 / rts;
 *           callers Snd_Init and Sound_RestartMusic (before every song change); 146 corpus runs.
 *           a1 only saved (dead stack, not an input)
 */
uint32_t Snd_StopChannels(void)
{
    return Snd_StopChannelsBody();
}
