; ---------------------------------------------------------------------------
; Fade_MaxTint - returns the largest of the 12 per-line tint offsets (r/g/b of palette lines 0-3), at least -$E
; In:  tint records $F4C7-$F4D5 (each 3 offset bytes + a flag byte, skipped)
; Out: d0.b = max (signed), a0 = $F4C7, d1 = -1
; Evidence: starts at $F2 and walks down from g_PalLineCache with -(a0), skipping every 4th byte; bgt keeps the larger; only caller Fade_StartOutBlack
; ---------------------------------------------------------------------------
Fade_MaxTint:
	move.b	#$F2,d0
	lea.l	(g_PalLineCache).l,a0
	moveq	#$3,d1
loc_00AB78:
	subq.l	#$1,a0
	cmp.b	-(a0),d0
	bgt.s	loc_00AB80
	move.b	(a0),d0
loc_00AB80:
	cmp.b	-(a0),d0
	bgt.s	loc_00AB86
	move.b	(a0),d0
loc_00AB86:
	cmp.b	-(a0),d0
	bgt.s	loc_00AB8C
	move.b	(a0),d0
loc_00AB8C:
	dbra	d1,loc_00AB78
	rts
