; ---------------------------------------------------------------------------
; HudOp_Halt - panel script op 0: stops the script on this op (timer 0, pointer kept)
; In:  a0 = script pointer after the op, a6 = player record
; Out: +$20 = the op, +$1E = 0
; Evidence: entry 0 of Tbl_011764; ends $10F84 (stats) and the GAME OVER script $10FD2
; ---------------------------------------------------------------------------
HudOp_Halt:
	lea.l	-$2(a0),a0
	move.l	a0,$20(a6)
	clr.w	$1E(a6)
	rts
