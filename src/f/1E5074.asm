; ---------------------------------------------------------------------------
; Snd_LoadDriver - driver API stub: copies the Z80 program into Z80 RAM and restarts the Z80 (jsr Snd_LoadDriverBody), a1 preserved
; In:  d1 = byte for Z80 RAM $003B (Snd_Init passes $E4)
; Out: as Snd_LoadDriverBody
; Evidence: only caller Snd_Init (boot); 146 corpus runs
; ---------------------------------------------------------------------------
Snd_LoadDriver:
	move.l	a1,-(a7)
	lea.l	(Snd_LoadDriverBody).l,a1
	jsr	(a1)
	movea.l	(a7)+,a1
	rts
