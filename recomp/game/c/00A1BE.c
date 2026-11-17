#include "game.h"

/* The music switch ($00A1BE-$00A1EF): three entries, each a test fewer than the one before, the
 * last restarting the Zyrinx driver's song. Each falls into the next on the 68000, so the C runs
 * the shared steps in place (plain C calls: no return address the asm doesn't push). */

/* the restart: the driver stopped and its fade reset, then the song started; a1-a6 saved on the
   stack around the driver calls (movem), as the asm does */
static void restart(uint32_t *song, uint32_t *d1, uint32_t *d7, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4,
                    uint32_t a5, uint32_t a6)
{
    uint32_t a0 = 0;                               /* clobbered: Snd_PlaySong leaves its module there */
    g_MusicCur = (uint16_t)*song;
    dk::push_long(a6);                             /* movem.l a1-a6,-(a7) */
    dk::push_long(a5);
    dk::push_long(a4);
    dk::push_long(a3);
    dk::push_long(a2);
    dk::push_long(a1);
    *d7 = Snd_StopChannels();
    *song = Snd_ResetFade();
    *song = (*song & 0xFFFF0000u) | g_MusicCur;
    if ((uint16_t)*song != 0)
        Snd_PlaySong(song, d1, d7, &a0, a1);
    for (int i = 0; i < 6; i++)
        (void)dk::pop_long();                      /* movem.l (a7)+,a1-a6: the bridge kept them */
}

/* the song test, then the restart */
static void play_unless_current(uint32_t *song, uint32_t *d1, uint32_t *d7, uint32_t a1, uint32_t a2, uint32_t a3,
                                uint32_t a4, uint32_t a5, uint32_t a6)
{
    if ((uint16_t)*song == g_MusicCur)
        return;
    restart(song, d1, d7, a1, a2, a3, a4, a5, a6);
}

/* Sound_PlayMusic - switches the music to `song` unless the music is locked (attract, title) or that
 * song is already playing.
 * In:  song (d0's low word); g_MusicLock, g_MusicCur
 * Out: as Sound_RestartMusic when it switches
 * Evidence: tst.b g_MusicLock / bne rts, then the fall into Sound_PlayMusicAnyway; 20 callers
 *           (Main_*, Stage_Intro*, the section routines $1FB3A-$47BD0, $433E)
 */
void Sound_PlayMusic(uint32_t *song, uint32_t *d1, uint32_t *d7, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4,
                     uint32_t a5, uint32_t a6)
{
    if (g_MusicLock)
        return;
    play_unless_current(song, d1, d7, a1, a2, a3, a4, a5, a6);
}

/* Sound_PlayMusicAnyway - Sound_PlayMusic without the lock test: plays `song` unless it is the
 * current one.
 * In:  song (d0's low word); g_MusicCur
 * Out: as Sound_RestartMusic when it switches
 * Evidence: cmp.w g_MusicCur,d0 / beq rts, then the fall into Sound_RestartMusic; caller $49712
 *           (the title, song $11, while the music is locked)
 */
void Sound_PlayMusicAnyway(uint32_t *song, uint32_t *d1, uint32_t *d7, uint32_t a1, uint32_t a2, uint32_t a3,
                           uint32_t a4, uint32_t a5, uint32_t a6)
{
    play_unless_current(song, d1, d7, a1, a2, a3, a4, a5, a6);
}

/* Sound_RestartMusic - (re)starts `song` unconditionally: g_MusicCur = it, the driver's channels
 * stopped and its fade reset, then (song != 0) the song started; a1-a6 kept (movem around it).
 * In:  song (d0's low word)
 * Out: g_MusicCur; song (d0) = Snd_ResetFade's d0 with g_MusicCur in its low word, then as
 *      Snd_PlaySong leaves it; d1 / d7 as the driver calls leave them
 * Evidence: move.w d0,g_MusicCur / movem.l a1-a6,-(a7) / jsr Snd_StopChannels / Snd_ResetFade /
 *           move.w g_MusicCur,d0 / beq / jsr Snd_PlaySong / movem.l (a7)+,a1-a6; caller $499E8
 *           (the options music test)
 */
void Sound_RestartMusic(uint32_t *song, uint32_t *d1, uint32_t *d7, uint32_t a1, uint32_t a2, uint32_t a3,
                        uint32_t a4, uint32_t a5, uint32_t a6)
{
    restart(song, d1, d7, a1, a2, a3, a4, a5, a6);
}

/* Sound_MusicDone - the rts the three entries' early exits share.
 * In:  none
 * Out: none
 * Evidence: rts at $A1EE, the target of the beq/bne in Sound_PlayMusic, Sound_PlayMusicAnyway and
 *           Sound_RestartMusic (the recompiler makes it a function of its own)
 */
void Sound_MusicDone(void)
{
}
