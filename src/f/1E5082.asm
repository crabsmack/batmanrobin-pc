; ---------------------------------------------------------------------------
; Snd_StopChannels - driver API stub: switches the six music channels off (jsr Snd_StopChannelsBody), a1 preserved
; In:  none
; Out: Z80 channel records $1480+$67n, +$1F = 0; d0/d7 trashed
; Evidence: callers Snd_Init and Sound_RestartMusic (before every song change, also for song 0 = silence); 146 corpus runs
; ---------------------------------------------------------------------------
Snd_StopChannels:
	move.l	a1,-(a7)
	lea.l	(Snd_StopChannelsBody).l,a1
	jsr	(a1)
	movea.l	(a7)+,a1
	rts
