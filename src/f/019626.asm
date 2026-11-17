; ---------------------------------------------------------------------------
; Boss_Harley_IsPartFlipped - script test of the Harley boss: returns 1 when its second sprite block ($E(a6)) is drawn H-flipped, else 0
; In:  a6 = object
; Out: d0 = 0 / 1 (for a native switch op)
; Evidence: btst #3,$38(a0) of the a5-layout block (attr byte bit 3 = H flip); entered from the boss scripts ($19474-) during the pole phase, TAS f5755-f5949 and 2 fuzz runs
; ---------------------------------------------------------------------------
Boss_Harley_IsPartFlipped:
	movea.w	$E(a6),a0
	btst	#$3,$38(a0)
	bne.s	loc_019636
	moveq	#$0,d0
	rts
loc_019636:
	moveq	#$1,d0
	rts
