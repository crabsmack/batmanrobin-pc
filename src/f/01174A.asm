; ---------------------------------------------------------------------------
; HUD_RunPanelScript - steps one player's HUD panel script: counts down the wait timer +$1E and, when it reaches 0, runs ops from +$20 through the op table Tbl_011764 until one waits
; In:  a6 = player record
; Out: +$1E/+$20 updated by the ops
; Evidence: called by HUD_Update for both records every frame; ops (word offsets into Tbl_011764): 0 HudOp_Halt, 4 HudOp_Wait, 8 HudOp_Goto, $C HudOp_SlideOut, $10 HudOp_SlideIn, $14 HudOp_WaitSlide, $18 HudOp_ShowText, $1C HudOp_ShowStats, $20 HudOp_ShowCredits
; ---------------------------------------------------------------------------
HUD_RunPanelScript:
	tst.w	$1E(a6)
	beq.s	loc_011756
	subq.w	#$1,$1E(a6)
	beq.s	loc_011758
loc_011756:
	rts
loc_011758:
	movea.l	$20(a6),a0
; ---------------------------------------------------------------------------
; HUD_PanelNext - the panel script's NEXT: reads the next op word and jumps through the op table Tbl_011764
; In:  a0 = script pointer at an op word, a6 = player record
; Out: a0 past the op word, d0 = the op's table offset, a1 = the handler, jumped to
; Evidence: move.w (a0)+,d0 / movea.l Tbl_011764(pc,d0.w),a1 / jmp (a1); entered by HUD_RunPanelScript and the continuing ops Goto/SlideOut/SlideIn/WaitSlide/ShowStats
; ---------------------------------------------------------------------------
HUD_PanelNext:
	move.w	(a0)+,d0
	movea.l	Tbl_011764(pc,d0.w),a1
	jmp	(a1)
