; ---------------------------------------------------------------------------
; Snd_WriteFadeByte - writes d0 into Z80 RAM $0E67 (the driver's fade/attenuation byte) with the Z80 bus held, repeating until it reads back
; In:  d0.b = value
; Out: Z80 bus released; a1 preserved
; Evidence: move.w #$100,$A11100 / wait bit 0 / move.b d0,$A00E67 with cmp.b read-back loop / release; callers Snd_ResetFadeBody, the never-run $1E79D4 and stub $1E5022; meaning of $0E67 from those callers (inferred)
; ---------------------------------------------------------------------------
Snd_WriteFadeByte:
	movem.l	a1,-(a7)
	move.w	#$100,($A11100).l
loc_1E79B0:
	btst	#$0,($A11100).l
	bne.s	loc_1E79B0
	lea.l	($A00E67).l,a1
loc_1E79C0:
	move.b	d0,(a1)
	cmp.b	(a1),d0
	bne.s	loc_1E79C0
	move.w	#$0,($A11100).l
	movem.l	(a7)+,a1
	rts
