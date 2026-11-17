#include "game.h"

/* Snd_StartSong - driver API stub: starts song d0 of the music module whose Tbl_1E7EE4 entry is
 * a0 (Snd_StartSongBody), a1 saved on the stack around the call
 * In:  module (a0) = the module's Tbl_1E7EE4 entry, song (d0) = the song in the module; a1 (pushed)
 * Out: as Snd_StartSongBody
 * Evidence: move.l a1,-(a7) / lea Snd_StartSongBody,a1 / jsr (a1) / movea.l (a7)+,a1 / rts;
 *           entered only by Snd_PlaySong (jmp); 146 corpus runs
 */
void Snd_StartSong(uint32_t *song, uint32_t *d1, uint32_t *d7, const SndModule *module, uint32_t a1)
{
    dk::push_long(a1);
    Snd_StartSongBody(song, d1, d7, module);
    (void)dk::pop_long();
}
