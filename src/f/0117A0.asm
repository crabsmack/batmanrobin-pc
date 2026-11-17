; ---------------------------------------------------------------------------
; HudOp_Goto - panel script op 8: jumps by the following word, relative to the op, and keeps running
; In:  a0 = operand pointer
; Out: a0 = target
; Evidence: entry 8 of Tbl_011764; closes the looping scripts $10F8A, $10FB2, $11012
; ---------------------------------------------------------------------------
HudOp_Goto:
	adda.w	(a0),a0
	lea.l	-$2(a0),a0
	bra.s	HUD_PanelNext
