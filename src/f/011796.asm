; ---------------------------------------------------------------------------
; HudOp_Wait - panel script op 4: waits the following word's number of frames
; In:  a0 = operand pointer, a6 = player record
; Out: +$1E = frames, +$20 = next op
; Evidence: entry 4 of Tbl_011764; 'wait 4' in Tbl_010F76
; ---------------------------------------------------------------------------
HudOp_Wait:
	move.w	(a0)+,$1E(a6)
	move.l	a0,$20(a6)
	rts
