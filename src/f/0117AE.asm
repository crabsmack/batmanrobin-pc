; ---------------------------------------------------------------------------
; HudOp_SlideIn - panel script op $10: starts the panel sliding back in from its screen edge (calls $1189C) and keeps running
; In:  a6 = player record
; Out: +$E start x, +$12/+$16 slide speed, +$25 bit 6 cleared
; Evidence: entry $10 of Tbl_011764; $1189C starts at home x -/+ $68 and moves toward +$A; used right after the content switch ($10F84, $10F8A) (direction inferred from the arithmetic)
; ---------------------------------------------------------------------------
HudOp_SlideIn:
	bsr.w	HUD_StartSlideIn
	bra.s	HUD_PanelNext
