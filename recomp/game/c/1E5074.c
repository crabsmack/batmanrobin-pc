#include "game.h"

/* Snd_LoadDriver - driver API stub: copies the Z80 program into Z80 RAM and restarts the Z80
 * (Snd_LoadDriverBody)
 * In:  d1 = the byte for Z80 RAM $003B (Snd_Init passes $E4)
 * Out: as Snd_LoadDriverBody (a5 = the end of the Z80 program in ROM)
 * Evidence: move.l a1,-(a7) / lea Snd_LoadDriverBody,a1 / jsr (a1) / movea.l (a7)+,a1 / rts; only
 *           caller Snd_Init (boot); 146 corpus runs. a1 only saved (dead stack, not an input)
 */
uint32_t Snd_LoadDriver(uint8_t d1)
{
    return Snd_LoadDriverBody(d1);
}
