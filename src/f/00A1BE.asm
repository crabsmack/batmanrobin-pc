; ---------------------------------------------------------------------------
; Sound_PlayMusic - switches the music to song d0 unless music is locked or that song is already playing
; In:  d0 = song (8/$C/$E/$B stages 1-4, $11 title, $12 ending, $13 continue/game over; 0 = silence)
; Out: $F622 = d0; a1-a6 preserved
; Evidence: skipped while $F6FE is set (attract/title, Main_Attract); compares $F622; 20 callers (Main_*, Stage_Intro*, the section routines $1FB3A-$47BD0, $433E); 29 TAS frames
; ---------------------------------------------------------------------------
Sound_PlayMusic:
	tst.b	(g_MusicLock).w
	bne.s	Sound_MusicDone
; ---------------------------------------------------------------------------
; Sound_PlayMusicAnyway - Sound_PlayMusic without the music-lock test: plays d0 unless it is the current song
; In:  d0 = song
; Out: as Sound_PlayMusic
; Evidence: caller $49712 (the title scene, song $11) while $F6FE is set
; ---------------------------------------------------------------------------
Sound_PlayMusicAnyway:
	cmp.w	(g_MusicCur).w,d0
	beq.s	Sound_MusicDone
; ---------------------------------------------------------------------------
; Sound_RestartMusic - (re)starts song d0 unconditionally: resets the driver ($1E5082, $1E500E) and, for d0 != 0, starts the song ($1E6012)
; In:  d0 = song
; Out: $F622 = d0
; Evidence: caller $499E8 (the options music test: d0 = $F60A + 1); $1E6012 is the start of the Zyrinx driver's 68k code (NOTES 'Sound'); driver routine roles inferred
; ---------------------------------------------------------------------------
Sound_RestartMusic:
	move.w	d0,(g_MusicCur).w
	movem.l	a1-a6,-(a7)
	jsr	(Snd_StopChannels).l
	jsr	(Snd_ResetFade).l
	move.w	(g_MusicCur).w,d0
	beq.s	loc_00A1EA
	jsr	(Snd_PlaySong).l
loc_00A1EA:
	movem.l	(a7)+,a1-a6
; ---------------------------------------------------------------------------
; Sound_MusicDone - the rts the music-switch entries' early exits share
; In:  none
; Out: none
; Evidence: target of the bne/beq in Sound_PlayMusic, Sound_PlayMusicAnyway and Sound_RestartMusic; the recompiler makes it a function of its own (P4 entries)
; ---------------------------------------------------------------------------
Sound_MusicDone:
	rts
