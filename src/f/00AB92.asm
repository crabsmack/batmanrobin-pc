; ---------------------------------------------------------------------------
; Fade_MinTint - returns the smallest of the 12 per-line tint offsets (r/g/b of palette lines 0-3), at most +$E
; In:  tint records $F4C7-$F4D5
; Out: d0.b = min (signed), a0 = $F4C7, d1 = -1
; Evidence: mirror of Fade_MaxTint starting at $E with blt; only caller Fade_StartOutWhite
; ---------------------------------------------------------------------------
Fade_MinTint:
	move.b	#$E,d0
	lea.l	(g_PalLineCache).l,a0
	moveq	#$3,d1
loc_00AB9E:
	subq.l	#$1,a0
	cmp.b	-(a0),d0
	blt.s	loc_00ABA6
	move.b	(a0),d0
loc_00ABA6:
	cmp.b	-(a0),d0
	blt.s	loc_00ABAC
	move.b	(a0),d0
loc_00ABAC:
	cmp.b	-(a0),d0
	blt.s	loc_00ABB2
	move.b	(a0),d0
loc_00ABB2:
	dbra	d1,loc_00AB9E
	rts
