; ---------------------------------------------------------------------------
; Snd_ResetFadeBody - driver body of Snd_ResetFade: clears the fade word at (a0) and writes 0 to the Z80's fade byte $0E67
; In:  a0 = g_SndFadeLevel
; Out: (a0).w = 0, d0 = 0
; Evidence: clr.w (a0) / moveq #0,d0 / bsr Snd_WriteFadeByte; the never-run $1E79D4 (stub $1E5030, same a0) instead adds d0 to (a0), clamps to $FF and sends curve byte $1E79F8[level] (0-$7F); 146 corpus runs
; ---------------------------------------------------------------------------
Snd_ResetFadeBody:
	clr.w	(a0)
	moveq	#$0,d0
	bsr.w	Snd_WriteFadeByte
	rts
