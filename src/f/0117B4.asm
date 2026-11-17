; ---------------------------------------------------------------------------
; HudOp_WaitSlide - panel script op $14: waits (re-running itself each frame) until the slide speed +$12 is 0
; In:  a0 = operand pointer, a6 = player record
; Out: +$20/+$1E = 1 while sliding
; Evidence: entry $14 of Tbl_011764; follows HudOp_SlideOut in Tbl_010F76, $10FB2, $10FD2
; ---------------------------------------------------------------------------
HudOp_WaitSlide:
	tst.l	$12(a6)
	beq.s	HUD_PanelNext
	subq.l	#$2,a0
	move.l	a0,$20(a6)
	move.w	#$1,$1E(a6)
	rts
