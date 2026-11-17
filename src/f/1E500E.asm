; ---------------------------------------------------------------------------
; Snd_ResetFade - driver API stub: resets the music fade (a0 = g_SndFadeLevel, jsr Snd_ResetFadeBody), a1 preserved
; In:  none
; Out: g_SndFadeLevel = 0, Z80 RAM $0E67 = 0; d0/a0 trashed
; Evidence: one of the jsr (a1) stubs at $1E500E-$1E50D6 that front the driver code at $1E799A-$1E7D8B; callers Snd_Init (bra) and Sound_RestartMusic (before every song start); 146 corpus runs
; ---------------------------------------------------------------------------
Snd_ResetFade:
	move.l	a1,-(a7)
	lea.l	(g_SndFadeLevel).l,a0
	lea.l	(Snd_ResetFadeBody).l,a1
	jsr	(a1)
	movea.l	(a7)+,a1
	rts
