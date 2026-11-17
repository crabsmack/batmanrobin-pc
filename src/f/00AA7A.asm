; ---------------------------------------------------------------------------
; Fade_FlickerTarget - moves g_FadeTarget one step (2) up or down at random, kept within -2..+$C: a flickering brightness when called every frame
; In:  g_FadeTarget
; Out: g_FadeTarget +-2; d0 = Rand_Next result
; Evidence: Rand_Next sign picks the direction, bounces off -2 and $C; pointers at $02D314 and $049516 (a2 native calls, the latter in a loop `dc.w $2CBC; dc.l $AA7A; dc.w $2D22, -$10`); 1 corpus run, not the TAS
; ---------------------------------------------------------------------------
Fade_FlickerTarget:
	bsr.w	Rand_Next
	bpl.s	loc_00AA8E
	cmpi.b	#$FE,(g_FadeTarget).w
	ble.s	loc_00AA96
loc_00AA88:
	subq.b	#$2,(g_FadeTarget).w
	rts
loc_00AA8E:
	cmpi.b	#$C,(g_FadeTarget).w
	bge.s	loc_00AA88
loc_00AA96:
	addq.b	#$2,(g_FadeTarget).w
	rts
